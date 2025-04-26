# Kullanıcı Ağı Yönetimi Projesi

Bu proje, kullanıcılar arasında arkadaşlık ilişkilerini modellemek, veri yapıları kullanarak etkili sorgular gerçekleştirmek ve sosyal ağ analizi yapmak amacıyla geliştirilmiştir.  
Red-Black Tree veri yapısı kullanılarak kullanıcılar hızlıca erişilebilir ve ağ üzerinde çeşitli analizler yapılabilir.

## Özellikler
- **Kullanıcı Ekleme:** `veriseti.txt` dosyasından kullanıcıları okuyarak sisteme ekler.
- **Arkadaşlık Bağlantıları:** Kullanıcılar arasında çift yönlü arkadaşlık bağlantıları kurar.
- **Belirli Mesafedeki Arkadaşları Bulma (DFS):** Bir kullanıcının belirli adım uzaklığındaki arkadaşlarını listeler.
- **Ortak Arkadaşları Bulma:** İki kullanıcının ortak arkadaşlarını tespit eder.
- **Topluluk (Connected Component) Tespiti:** Ağ üzerindeki bağımsız toplulukların (connected components) sayısını bulur.
- **Etkileşim Alanı (Influence Area):** Bir kullanıcının doğrudan veya dolaylı erişebildiği kişi sayısını hesaplar.

## Kullanılan Veri Yapıları
- **Red-Black Tree:** Kullanıcıları hızlıca eklemek ve aramak için kullanıldı.
- **Dinamik Dizi:** Her kullanıcının arkadaş listesini tutmak için kullanıldı.

## Dosya Yapısı
- `veriseti.txt`: Kullanıcı ve arkadaşlık verilerini içeren giriş dosyası.
- `main.c`: Tüm kaynak kodlarını ve algoritmaları içeren dosya.

## Veri Dosyası Formatı (`veriseti.txt`)
```
USER <id>         // Yeni kullanıcı ekler
FRIEND <id1> <id2> // İki kullanıcı arasında arkadaşlık ilişkisi kurar
```
Örnek:
```
USER 101
USER 102
USER 103
FRIEND 101 102
FRIEND 102 103
```

## Derleme ve Çalıştırma
Projenin çalıştırılması için temel C derleyicisi yeterlidir.

### Derlemek için:
```bash
gcc main.c -o sosyal_ag
```

### Çalıştırmak için:
```bash
./sosyal_ag
```

## Notlar
- Sistem maksimum 1000 kullanıcıyı destekleyecek şekilde ayarlanmıştır.
- Her kullanıcı için arkadaş listesi dinamik olarak genişletilmektedir.
- Red-Black Tree sayesinde kullanıcı ekleme ve arama işlemleri hızlı bir şekilde gerçekleştirilir.

## Geliştirici Notları
- İleride kullanıcı isimleri ve diğer kullanıcı bilgileri de eklenebilir.
- Toplulukların büyüklükleri veya en etkili kullanıcı gibi gelişmiş analizler eklenebilir.
- Dosya okuma kısmı hata yönetimi açısından daha sağlam hale getirilebilir.
