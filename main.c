#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GUNLUK_DOSYA "/var/log/syslog" // Syslog dosyasýnýn yolu

// Çift yönlü baðlý liste düðümünü tanýmlýyoruz
typedef struct GunlukDugum {
    char gunluk_kaydi[512]; // Günlük kaydýný saklamak için karakter dizisi
    struct GunlukDugum* onceki; // Önceki düðüme iþaretçi
    struct GunlukDugum* sonraki; // Sonraki düðüme iþaretçi
} GunlukDugum;

// Baðlý liste baþlangýç ve bitiþ noktalarýný tanýmlýyoruz
GunlukDugum* bas = NULL; 
GunlukDugum* son = NULL; 

// Yeni bir günlük kaydý ekleme fonksiyonu
void gunluk_ekle(const char* kayit) {
    // Yeni bir düðüm için bellek tahsis ediyoruz
    GunlukDugum* yeni_dugum = (GunlukDugum*)malloc(sizeof(GunlukDugum));
    if (!yeni_dugum) {
        perror("Bellek tahsis hatasi");  
        return;
    }
    // Günlük kaydýný düðüme kopyalýyoruz
    strncpy(yeni_dugum->gunluk_kaydi, kayit, 511);
    yeni_dugum->gunluk_kaydi[511] = '\0'; // Son karakteri null terminatör olarak ayarlýyoruz
    yeni_dugum->sonraki = NULL;
    
    if (!bas) { // Eðer liste boþsa baþa
        yeni_dugum->onceki = NULL;
        bas = son = yeni_dugum; 
    } else { // Liste doluysa sona 
        son->sonraki = yeni_dugum;  
        yeni_dugum->onceki = son; 
        son = yeni_dugum; 
    }
}

// Syslog dosyasýný okuyarak baðlý listeye ekleme fonksiyonu
void gunlukleri_oku() {
    FILE* dosya = fopen(GUNLUK_DOSYA, "r"); // Syslog dosyasýný okuma modunda açýyoruz
    if (!dosya) {
        perror("Syslog dosyasi acilamadi");
        return;
    }
    char tampon[512]; // Okunan satýrý geçici olarak saklamak için
    while (fgets(tampon, sizeof(tampon), dosya)) { // Satýr satýr oku
        gunluk_ekle(tampon); // Baðlý listeye ekle
    }
    fclose(dosya); // Dosyayý kapat
}

// Baðlý listedeki günlük kayýtlarýný yazdýrma fonksiyonu
void gunlukleri_yazdir() {
    GunlukDugum* gecici = bas; // Listenin baþýndan baþlayarak ilerle
    while (gecici) { // Liste sonuna kadar git
        printf("%s", gecici->gunluk_kaydi); // Günlük kaydýný yazdýr
        gecici = gecici->sonraki; // Sonraki düðüme geç
    }
}

// Belleði temizleme fonksiyonu  amaç kullanýlan belleði temizleyerek bellek sýzýntýsýný önlemektir
void gunlukleri_temizle() {
    GunlukDugum* gecici; // Geçici bir düðüm iþaretçisi tanýmlýyoruz
    while (bas) { // Baðlý listeyi baþtan sona temizle
        gecici = bas; // Ýlk düðümü sakla
        bas = bas->sonraki; // Baþý bir sonraki düðüme kaydýr
        free(gecici); // Belleði serbest býrak
    }
    son = NULL; // Son düðümü de sýfýrla
}

int main() {
    gunlukleri_oku(); // Syslog dosyasýný oku ve baðlý listeye ekle
    gunlukleri_yazdir(); // Günlük kayýtlarýný ekrana yazdýr
    gunlukleri_temizle(); // Belleði temizle
    return 0;
}

