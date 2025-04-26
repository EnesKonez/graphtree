#include <stdio.h>   // Girdi/��kt� i�lemleri i�in gerekli k�t�phane
#include <stdlib.h>  // Bellek y�netimi (malloc, realloc, free) i�in gerekli k�t�phane
#include <string.h>  // String i�lemleri (strncmp, memset gibi) i�in gerekli k�t�phane

// Kullan�c�y� temsil eden yap� (struct)
typedef struct User {
    int id;                          // Kullan�c�n�n kimlik numaras�
    struct User** friends;            // Kullan�c�n�n arkada�lar�n� tutacak dinamik dizi
    int friendCount;                  // Kullan�c�n�n arkada� say�s�
} User;

// Red-Black Tree d���m�n� temsil eden yap�
typedef struct RBNode {
    int id;                           // D���m�n kimlik de�eri (ayn� zamanda kullan�c� kimli�i)
    User* user;                       // D���me ait kullan�c�ya i�aret�i
    struct RBNode *left, *right;       // Sol ve sa� �ocuk d���mler
    int color;                        // D���m�n rengi (RED:1 veya BLACK:0)
} RBNode;

#define RED 1     // K�rm�z� rengi temsil eder
#define BLACK 0   // Siyah rengi temsil eder

User* users[1000];  // Kullan�c�lar� ID'lerine g�re saklayan dizi (en fazla 1000 kullan�c�)

RBNode* root = NULL;  // Red-Black a�ac�n�n k�k d���m�

/* === Yard�mc� Fonksiyonlar === */

// Yeni bir kullan�c� olu�turur
User* createUser(int id) {
    User* user = (User*)malloc(sizeof(User));  // Yeni kullan�c� i�in bellek ay�r�r
    user->id = id;          // Kimlik atamas� yap�l�r
    user->friends = NULL;   // Ba�lang��ta arkada� listesi bo�
    user->friendCount = 0;  // Arkada� say�s� 0
    return user;            // Kullan�c�y� d�nd�r
}

// �ki kullan�c� aras�nda arkada�l�k ba�lant�s� kurar
void addFriend(User* a, User* b) {
    a->friends = (User**)realloc(a->friends, sizeof(User*) * (a->friendCount + 1)); // Arkada� listesini geni�letir
    a->friends[a->friendCount++] = b;  // Yeni arkada�� listeye ekler ve saya� art�r�l�r
}

// Yeni bir Red-Black Tree d���m� olu�turur
RBNode* createRBNode(User* user) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));  // Bellek ay�r�r
    node->id = user->id;             // Kimlik atar
    node->user = user;               // Kullan�c�y� d���me ba�lar
    node->left = node->right = NULL;  // �ocuk d���mler ba�lang��ta bo�
    node->color = RED;                // Yeni eklenen d���m k�rm�z� olur
    return node;                      // Yeni d���m� d�nd�r�r
}

// Red-Black Tree'de sola d�n�� i�lemi yapar
RBNode* rotateLeft(RBNode* node) {
    RBNode* r = node->right;        // Sa� �ocu�u al�r
    node->right = r->left;          // Sa� �ocu�un solunu node'un sa��na koyar
    r->left = node;                 // Node'u sa� �ocu�un solu yapar
    r->color = node->color;         // Renkleri kopyalar
    node->color = RED;              // Node k�rm�z�ya d�ner
    return r;                       // Yeni k�k� d�nd�r�r
}

// Red-Black Tree'de sa�a d�n�� i�lemi yapar
RBNode* rotateRight(RBNode* node) {
    RBNode* l = node->left;         // Sol �ocu�u al�r
    node->left = l->right;          // Sol �ocu�un sa��n� node'un soluna koyar
    l->right = node;                // Node'u sol �ocu�un sa��na koyar
    l->color = node->color;         // Renkleri kopyalar
    node->color = RED;              // Node k�rm�z�ya d�ner
    return l;                       // Yeni k�k� d�nd�r�r
}

// D���m ve �ocuklar�n�n renklerini de�i�tirir
void flipColors(RBNode* node) {
    node->color = RED;                // Node k�rm�z� yap�l�r
    if(node->left) node->left->color = BLACK;   // Sol �ocuk siyah yap�l�r
    if(node->right) node->right->color = BLACK; // Sa� �ocuk siyah yap�l�r
}

// Bir d���m k�rm�z� m� diye kontrol eder
int isRed(RBNode* node) {
    if (node == NULL) return 0;        // Null ise k�rm�z� de�ildir
    return node->color == RED;          // Rengi kontrol edilir
}

// Red-Black Tree'ye kullan�c� ekler
RBNode* insertRB(RBNode* node, User* user) {
    if (node == NULL) return createRBNode(user);  // Bo�sa yeni d���m olu�tur

    if (user->id < node->id)        // Sol alt a�aca ekle
        node->left = insertRB(node->left, user);
    else if (user->id > node->id)   // Sa� alt a�aca ekle
        node->right = insertRB(node->right, user);

    // Red-Black kurallar�na g�re d�n�� ve renk de�i�imi yap
    if (isRed(node->right) && !isRed(node->left)) node = rotateLeft(node);
    if (isRed(node->left) && isRed(node->left->left)) node = rotateRight(node);
    if (isRed(node->left) && isRed(node->right)) flipColors(node);

    return node;  // G�ncellenmi� node'u d�nd�r
}

// Verileri dosyadan okur
void loadData(const char* filename) {
    FILE* file = fopen(filename, "r");  // Dosyay� okuma modunda a�
    if (!file) {                        // Dosya a��lamazsa
        printf("Dosya bulunamadi!\n");
        return;
    }

    char line[256];  // Sat�r okuma i�in tampon
    while (fgets(line, sizeof(line), file)) {   // Her sat�r� oku
        if (strncmp(line, "USER", 4) == 0) {     // Sat�r "USER" ile ba�l�yorsa
            int id;
            sscanf(line + 5, "%d", &id);         // Kullan�c� ID'sini �ek
            users[id] = createUser(id);          // Kullan�c� olu�tur
            root = insertRB(root, users[id]);    // A�aca ekle
        }
        else if (strncmp(line, "FRIEND", 6) == 0) {  // Sat�r "FRIEND" ile ba�l�yorsa
            int id1, id2;
            sscanf(line + 7, "%d %d", &id1, &id2); // �ki kullan�c�n�n ID'sini �ek
            addFriend(users[id1], users[id2]);     // Birbirlerini arkada� yap
            addFriend(users[id2], users[id1]);     // �ift y�nl� ba�lant�
        }
    }

    fclose(file);  // Dosyay� kapat
}

// Derinlik S�n�rl� Arama (DFS) ile belirli mesafedeki arkada�lar� bulur
void dfs(User* user, int depth, int currentDepth, int* visited) {
    if (currentDepth == depth) {           // �stenilen derinli�e ula��ld�ysa
        printf("Kullanici ID: %d\n", user->id);  // Kullan�c�y� yazd�r
        return;
    }

    visited[user->id] = 1;  // Ziyaret edildi olarak i�aretle
	int i;
    for ( i = 0; i < user->friendCount; i++) {  // T�m arkada�lar�n� dola�
        if (!visited[user->friends[i]->id]) {      // E�er ziyaret edilmediyse
            dfs(user->friends[i], depth, currentDepth + 1, visited);  // Derinli�i art�rarak devam et
        }
    }

    visited[user->id] = 0;  // Geriye d�nerken ziyaret bilgisini s�f�rla
}

// �ki kullan�c� aras�nda ortak arkada�lar� bulur
void findCommonFriends(User* a, User* b) {
    int j;
	int i;
	printf("Ortak Arkadaslar (%d ve %d): ", a->id, b->id);  // Ba�l�k yazd�r
    for (i = 0; i < a->friendCount; i++) {   // A'n�n t�m arkada�lar�n� dola�
        for ( j = 0; j < b->friendCount; j++) {  // B'nin t�m arkada�lar�n� dola�
            if (a->friends[i]->id == b->friends[j]->id) {  // ID'ler e�le�iyorsa
                printf("%d ", a->friends[i]->id);          // Ortak arkada�� yazd�r
            }
        }
    }
    printf("\n");
}

// Topluluklar� (ba�l� bile�enleri) bulmak i�in DFS
void dfsCommunity(User* user, int* visited) {
    
	int i;
	visited[user->id] = 1;  // Kullan�c�y� ziyaret edildi olarak i�aretle
    for ( i = 0; i < user->friendCount; i++) {  // T�m arkada�lar�n� dola�
        if (!visited[user->friends[i]->id]) {      // Hen�z ziyaret edilmediyse
            dfsCommunity(user->friends[i], visited);  // Topluluk i�inde devam et
        }
    }
}

// Toplam topluluk (ba�l� bile�en) say�s�n� bulur
void findCommunities() {
    int visited[1000] = {0};  // Ziyaret kontrol dizisi
    int i;
	int count = 0;            // Topluluk sayac�
    for ( i = 0; i < 1000; i++) {  // T�m kullan�c�lar� dola�
        if (users[i] && !visited[i]) {   // E�er kullan�c� varsa ve ziyaret edilmediyse
            dfsCommunity(users[i], visited);  // DFS ile toplulu�u ke�fet
            count++;  // Bir topluluk bulundu
        }
    }
    printf("Toplam Topluluk Sayisi: %d\n", count);  // Sonucu yazd�r
}

// Bir kullan�c�n�n etkiledi�i ki�i say�s�n� hesaplar
int calculateInfluence(User* user, int* visited) {
    int count = 1;  // Kullan�c�n�n kendisi dahil
    visited[user->id] = 1;  // Kullan�c�y� ziyaret edildi i�aretle
    int i;
	for ( i = 0; i < user->friendCount; i++) {  // T�m arkada�lar�n� dola�
        if (!visited[user->friends[i]->id]) {      // E�er hen�z ziyaret edilmemi�se
            count += calculateInfluence(user->friends[i], visited);  // Etki alan�n� art�r
        }
    }
    return count;  // Etki alan�n� d�nd�r
}

/* === Ana Fonksiyon === */

int main() {
    loadData("veriseti.txt");  // Verileri dosyadan oku

    printf("\n-- Belirli Mesafedeki Arkadaslar (DFS) --\n");
    int visited[1000] = {0};  // Ziyaret kontrol dizisi
    dfs(users[101], 2, 0, visited);  // Kullan�c� 101 i�in 2 ad�m uzakl�ktaki arkada�lar� bul

    printf("\n-- Ortak Arkadaslar --\n");
    findCommonFriends(users[101], users[102]);  // 101 ve 102 numaral� kullan�c�lar�n ortak arkada�lar�n� bul

    printf("\n-- Topluluk Tespiti --\n");
    findCommunities();  // A��daki topluluk say�s�n� bul

    printf("\n-- Etki Alanlari --\n");
    memset(visited, 0, sizeof(visited));  // Ziyaret bilgilerini s�f�rla
    int influence = calculateInfluence(users[101], visited);  // Kullan�c� 101'in etki alan�n� hesapla
    printf("Kullanici 101'in Etki Alani: %d kisi\n", influence);  // Etki alan�n� yazd�r

    return 0;  // Program� ba�ar�yla bitir
}

