#include <stdio.h>   // Girdi/Çýktý iþlemleri için gerekli kütüphane
#include <stdlib.h>  // Bellek yönetimi (malloc, realloc, free) için gerekli kütüphane
#include <string.h>  // String iþlemleri (strncmp, memset gibi) için gerekli kütüphane

// Kullanýcýyý temsil eden yapý (struct)
typedef struct User {
    int id;                          // Kullanýcýnýn kimlik numarasý
    struct User** friends;            // Kullanýcýnýn arkadaþlarýný tutacak dinamik dizi
    int friendCount;                  // Kullanýcýnýn arkadaþ sayýsý
} User;

// Red-Black Tree düðümünü temsil eden yapý
typedef struct RBNode {
    int id;                           // Düðümün kimlik deðeri (ayný zamanda kullanýcý kimliði)
    User* user;                       // Düðüme ait kullanýcýya iþaretçi
    struct RBNode *left, *right;       // Sol ve sað çocuk düðümler
    int color;                        // Düðümün rengi (RED:1 veya BLACK:0)
} RBNode;

#define RED 1     // Kýrmýzý rengi temsil eder
#define BLACK 0   // Siyah rengi temsil eder

User* users[1000];  // Kullanýcýlarý ID'lerine göre saklayan dizi (en fazla 1000 kullanýcý)

RBNode* root = NULL;  // Red-Black aðacýnýn kök düðümü

/* === Yardýmcý Fonksiyonlar === */

// Yeni bir kullanýcý oluþturur
User* createUser(int id) {
    User* user = (User*)malloc(sizeof(User));  // Yeni kullanýcý için bellek ayýrýr
    user->id = id;          // Kimlik atamasý yapýlýr
    user->friends = NULL;   // Baþlangýçta arkadaþ listesi boþ
    user->friendCount = 0;  // Arkadaþ sayýsý 0
    return user;            // Kullanýcýyý döndür
}

// Ýki kullanýcý arasýnda arkadaþlýk baðlantýsý kurar
void addFriend(User* a, User* b) {
    a->friends = (User**)realloc(a->friends, sizeof(User*) * (a->friendCount + 1)); // Arkadaþ listesini geniþletir
    a->friends[a->friendCount++] = b;  // Yeni arkadaþý listeye ekler ve sayaç artýrýlýr
}

// Yeni bir Red-Black Tree düðümü oluþturur
RBNode* createRBNode(User* user) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));  // Bellek ayýrýr
    node->id = user->id;             // Kimlik atar
    node->user = user;               // Kullanýcýyý düðüme baðlar
    node->left = node->right = NULL;  // Çocuk düðümler baþlangýçta boþ
    node->color = RED;                // Yeni eklenen düðüm kýrmýzý olur
    return node;                      // Yeni düðümü döndürür
}

// Red-Black Tree'de sola dönüþ iþlemi yapar
RBNode* rotateLeft(RBNode* node) {
    RBNode* r = node->right;        // Sað çocuðu alýr
    node->right = r->left;          // Sað çocuðun solunu node'un saðýna koyar
    r->left = node;                 // Node'u sað çocuðun solu yapar
    r->color = node->color;         // Renkleri kopyalar
    node->color = RED;              // Node kýrmýzýya döner
    return r;                       // Yeni kökü döndürür
}

// Red-Black Tree'de saða dönüþ iþlemi yapar
RBNode* rotateRight(RBNode* node) {
    RBNode* l = node->left;         // Sol çocuðu alýr
    node->left = l->right;          // Sol çocuðun saðýný node'un soluna koyar
    l->right = node;                // Node'u sol çocuðun saðýna koyar
    l->color = node->color;         // Renkleri kopyalar
    node->color = RED;              // Node kýrmýzýya döner
    return l;                       // Yeni kökü döndürür
}

// Düðüm ve çocuklarýnýn renklerini deðiþtirir
void flipColors(RBNode* node) {
    node->color = RED;                // Node kýrmýzý yapýlýr
    if(node->left) node->left->color = BLACK;   // Sol çocuk siyah yapýlýr
    if(node->right) node->right->color = BLACK; // Sað çocuk siyah yapýlýr
}

// Bir düðüm kýrmýzý mý diye kontrol eder
int isRed(RBNode* node) {
    if (node == NULL) return 0;        // Null ise kýrmýzý deðildir
    return node->color == RED;          // Rengi kontrol edilir
}

// Red-Black Tree'ye kullanýcý ekler
RBNode* insertRB(RBNode* node, User* user) {
    if (node == NULL) return createRBNode(user);  // Boþsa yeni düðüm oluþtur

    if (user->id < node->id)        // Sol alt aðaca ekle
        node->left = insertRB(node->left, user);
    else if (user->id > node->id)   // Sað alt aðaca ekle
        node->right = insertRB(node->right, user);

    // Red-Black kurallarýna göre dönüþ ve renk deðiþimi yap
    if (isRed(node->right) && !isRed(node->left)) node = rotateLeft(node);
    if (isRed(node->left) && isRed(node->left->left)) node = rotateRight(node);
    if (isRed(node->left) && isRed(node->right)) flipColors(node);

    return node;  // Güncellenmiþ node'u döndür
}

// Verileri dosyadan okur
void loadData(const char* filename) {
    FILE* file = fopen(filename, "r");  // Dosyayý okuma modunda aç
    if (!file) {                        // Dosya açýlamazsa
        printf("Dosya bulunamadi!\n");
        return;
    }

    char line[256];  // Satýr okuma için tampon
    while (fgets(line, sizeof(line), file)) {   // Her satýrý oku
        if (strncmp(line, "USER", 4) == 0) {     // Satýr "USER" ile baþlýyorsa
            int id;
            sscanf(line + 5, "%d", &id);         // Kullanýcý ID'sini çek
            users[id] = createUser(id);          // Kullanýcý oluþtur
            root = insertRB(root, users[id]);    // Aðaca ekle
        }
        else if (strncmp(line, "FRIEND", 6) == 0) {  // Satýr "FRIEND" ile baþlýyorsa
            int id1, id2;
            sscanf(line + 7, "%d %d", &id1, &id2); // Ýki kullanýcýnýn ID'sini çek
            addFriend(users[id1], users[id2]);     // Birbirlerini arkadaþ yap
            addFriend(users[id2], users[id1]);     // Çift yönlü baðlantý
        }
    }

    fclose(file);  // Dosyayý kapat
}

// Derinlik Sýnýrlý Arama (DFS) ile belirli mesafedeki arkadaþlarý bulur
void dfs(User* user, int depth, int currentDepth, int* visited) {
    if (currentDepth == depth) {           // Ýstenilen derinliðe ulaþýldýysa
        printf("Kullanici ID: %d\n", user->id);  // Kullanýcýyý yazdýr
        return;
    }

    visited[user->id] = 1;  // Ziyaret edildi olarak iþaretle
	int i;
    for ( i = 0; i < user->friendCount; i++) {  // Tüm arkadaþlarýný dolaþ
        if (!visited[user->friends[i]->id]) {      // Eðer ziyaret edilmediyse
            dfs(user->friends[i], depth, currentDepth + 1, visited);  // Derinliði artýrarak devam et
        }
    }

    visited[user->id] = 0;  // Geriye dönerken ziyaret bilgisini sýfýrla
}

// Ýki kullanýcý arasýnda ortak arkadaþlarý bulur
void findCommonFriends(User* a, User* b) {
    int j;
	int i;
	printf("Ortak Arkadaslar (%d ve %d): ", a->id, b->id);  // Baþlýk yazdýr
    for (i = 0; i < a->friendCount; i++) {   // A'nýn tüm arkadaþlarýný dolaþ
        for ( j = 0; j < b->friendCount; j++) {  // B'nin tüm arkadaþlarýný dolaþ
            if (a->friends[i]->id == b->friends[j]->id) {  // ID'ler eþleþiyorsa
                printf("%d ", a->friends[i]->id);          // Ortak arkadaþý yazdýr
            }
        }
    }
    printf("\n");
}

// Topluluklarý (baðlý bileþenleri) bulmak için DFS
void dfsCommunity(User* user, int* visited) {
    
	int i;
	visited[user->id] = 1;  // Kullanýcýyý ziyaret edildi olarak iþaretle
    for ( i = 0; i < user->friendCount; i++) {  // Tüm arkadaþlarýný dolaþ
        if (!visited[user->friends[i]->id]) {      // Henüz ziyaret edilmediyse
            dfsCommunity(user->friends[i], visited);  // Topluluk içinde devam et
        }
    }
}

// Toplam topluluk (baðlý bileþen) sayýsýný bulur
void findCommunities() {
    int visited[1000] = {0};  // Ziyaret kontrol dizisi
    int i;
	int count = 0;            // Topluluk sayacý
    for ( i = 0; i < 1000; i++) {  // Tüm kullanýcýlarý dolaþ
        if (users[i] && !visited[i]) {   // Eðer kullanýcý varsa ve ziyaret edilmediyse
            dfsCommunity(users[i], visited);  // DFS ile topluluðu keþfet
            count++;  // Bir topluluk bulundu
        }
    }
    printf("Toplam Topluluk Sayisi: %d\n", count);  // Sonucu yazdýr
}

// Bir kullanýcýnýn etkilediði kiþi sayýsýný hesaplar
int calculateInfluence(User* user, int* visited) {
    int count = 1;  // Kullanýcýnýn kendisi dahil
    visited[user->id] = 1;  // Kullanýcýyý ziyaret edildi iþaretle
    int i;
	for ( i = 0; i < user->friendCount; i++) {  // Tüm arkadaþlarýný dolaþ
        if (!visited[user->friends[i]->id]) {      // Eðer henüz ziyaret edilmemiþse
            count += calculateInfluence(user->friends[i], visited);  // Etki alanýný artýr
        }
    }
    return count;  // Etki alanýný döndür
}

/* === Ana Fonksiyon === */

int main() {
    loadData("veriseti.txt");  // Verileri dosyadan oku

    printf("\n-- Belirli Mesafedeki Arkadaslar (DFS) --\n");
    int visited[1000] = {0};  // Ziyaret kontrol dizisi
    dfs(users[101], 2, 0, visited);  // Kullanýcý 101 için 2 adým uzaklýktaki arkadaþlarý bul

    printf("\n-- Ortak Arkadaslar --\n");
    findCommonFriends(users[101], users[102]);  // 101 ve 102 numaralý kullanýcýlarýn ortak arkadaþlarýný bul

    printf("\n-- Topluluk Tespiti --\n");
    findCommunities();  // Aðýdaki topluluk sayýsýný bul

    printf("\n-- Etki Alanlari --\n");
    memset(visited, 0, sizeof(visited));  // Ziyaret bilgilerini sýfýrla
    int influence = calculateInfluence(users[101], visited);  // Kullanýcý 101'in etki alanýný hesapla
    printf("Kullanici 101'in Etki Alani: %d kisi\n", influence);  // Etki alanýný yazdýr

    return 0;  // Programý baþarýyla bitir
}

