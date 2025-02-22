#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GUNLUK_DOSYA "/var/log/syslog" // Syslog dosyas�n�n yolu

// �ift y�nl� ba�l� liste d���m�n� tan�ml�yoruz
typedef struct GunlukDugum {
    char gunluk_kaydi[512]; // G�nl�k kayd�n� saklamak i�in karakter dizisi
    struct GunlukDugum* onceki; // �nceki d���me i�aret�i
    struct GunlukDugum* sonraki; // Sonraki d���me i�aret�i
} GunlukDugum;

// Ba�l� liste ba�lang�� ve biti� noktalar�n� tan�ml�yoruz
GunlukDugum* bas = NULL; 
GunlukDugum* son = NULL; 

// Yeni bir g�nl�k kayd� ekleme fonksiyonu
void gunluk_ekle(const char* kayit) {
    // Yeni bir d���m i�in bellek tahsis ediyoruz
    GunlukDugum* yeni_dugum = (GunlukDugum*)malloc(sizeof(GunlukDugum));
    if (!yeni_dugum) {
        perror("Bellek tahsis hatasi");  
        return;
    }
    // G�nl�k kayd�n� d���me kopyal�yoruz
    strncpy(yeni_dugum->gunluk_kaydi, kayit, 511);
    yeni_dugum->gunluk_kaydi[511] = '\0'; // Son karakteri null terminat�r olarak ayarl�yoruz
    yeni_dugum->sonraki = NULL;
    
    if (!bas) { // E�er liste bo�sa ba�a
        yeni_dugum->onceki = NULL;
        bas = son = yeni_dugum; 
    } else { // Liste doluysa sona 
        son->sonraki = yeni_dugum;  
        yeni_dugum->onceki = son; 
        son = yeni_dugum; 
    }
}

// Syslog dosyas�n� okuyarak ba�l� listeye ekleme fonksiyonu
void gunlukleri_oku() {
    FILE* dosya = fopen(GUNLUK_DOSYA, "r"); // Syslog dosyas�n� okuma modunda a��yoruz
    if (!dosya) {
        perror("Syslog dosyasi acilamadi");
        return;
    }
    char tampon[512]; // Okunan sat�r� ge�ici olarak saklamak i�in
    while (fgets(tampon, sizeof(tampon), dosya)) { // Sat�r sat�r oku
        gunluk_ekle(tampon); // Ba�l� listeye ekle
    }
    fclose(dosya); // Dosyay� kapat
}

// Ba�l� listedeki g�nl�k kay�tlar�n� yazd�rma fonksiyonu
void gunlukleri_yazdir() {
    GunlukDugum* gecici = bas; // Listenin ba��ndan ba�layarak ilerle
    while (gecici) { // Liste sonuna kadar git
        printf("%s", gecici->gunluk_kaydi); // G�nl�k kayd�n� yazd�r
        gecici = gecici->sonraki; // Sonraki d���me ge�
    }
}

// Belle�i temizleme fonksiyonu  ama� kullan�lan belle�i temizleyerek bellek s�z�nt�s�n� �nlemektir
void gunlukleri_temizle() {
    GunlukDugum* gecici; // Ge�ici bir d���m i�aret�isi tan�ml�yoruz
    while (bas) { // Ba�l� listeyi ba�tan sona temizle
        gecici = bas; // �lk d���m� sakla
        bas = bas->sonraki; // Ba�� bir sonraki d���me kayd�r
        free(gecici); // Belle�i serbest b�rak
    }
    son = NULL; // Son d���m� de s�f�rla
}

int main() {
    gunlukleri_oku(); // Syslog dosyas�n� oku ve ba�l� listeye ekle
    gunlukleri_yazdir(); // G�nl�k kay�tlar�n� ekrana yazd�r
    gunlukleri_temizle(); // Belle�i temizle
    return 0;
}

