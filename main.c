#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT 10 /*valor fixo para a opção que finaliza a aplicação*/

// Struct que representa a data.
typedef struct {
  int day;
  int month;
  int year;
} Date;

// Estrutura que contém os campos dos registros da agenda
struct MREC {
  char name[30];
  Date birth;
  char email[40];
  char phone[15];
  int height;
  struct MREC *left;
  struct MREC *right;
};

// Tipo criado para instanciar variaveis do tipo agenda
typedef struct MREC Contact;

int max(int a, int b) { return (a > b) ? a : b; }

int height(Contact *node) {
  if (node == NULL)
    return 0;
  return 1 + max(height(node->left), height(node->right));
}

Contact *newNode(char name[30], Date birth, char email[40], char phone[15]) {
  Contact *node = (Contact *)malloc(sizeof(Contact));
  strcpy(node->name, name);
  node->birth = birth;
  strcpy(node->email, email);
  strcpy(node->phone, phone);
  node->left = NULL;
  node->right = NULL;
  node->height = 0;

  return node;
}

Contact *leftRotate(Contact *x) {
  Contact *y = x->right;
  Contact *T2 = y->left;

  y->left = x;
  x->right = T2;

  //  novas alturas
  x->height = height(x);
  y->height = height(y);

  return y;
}

Contact *rightRotate(Contact *y) {
  Contact *x = y->left;
  Contact *T2 = x->right;

  x->right = y;
  y->left = T2;

  //  novas alturas
  y->height = height(y);
  x->height = height(x);

  return x;
}

int getBalance(Contact *node) {
  if (node == NULL)
    return 0;
  return height(node->left) - height(node->right);
}

Contact *insContact(Contact *node, char name[30], Date birth, char email[40],
                    char phone[15]) {

  if (node == NULL)
    return (newNode(name, birth, email, phone));

  if (strcmp(name, node->name) < 0) // if name appears before node->name
    node->left = insContact(node->left, name, birth, email, phone);
  else if (strcmp(name, node->name) > 0) // if name appears before node->name
    node->right = insContact(node->right, name, birth, email, phone);
  else // nao pode ter nomes iguais
    printf("Contato com esse nome já foi registrado\n");
  return node;

  // nova altura
  node->height = height(node);

  // Checando se a arvore ficou desbalanceado
  int balance = getBalance(node);
  // Left Left Case
  if (balance > 1 && strcmp(name, node->left->name) < 0)
    return rightRotate(node);

  // Right Right Case
  if (balance < -1 && strcmp(name, node->left->name) < 0)
    return leftRotate(node);

  // Left Right Case
  if (balance > 1 && strcmp(name, node->left->name) > 0) {
    node->left = leftRotate(node->left);
    return rightRotate(node);
  }
  // Right Left Case
  if (balance < -1 && strcmp(name, node->right->name) < 0) {
    node->right = rightRotate(node->right);
    return leftRotate(node);
  }

  return node;
}

Contact *minValueNode(Contact *node) {
  Contact *current = node;

  while (current->left != NULL)
    current = current->left;

  return current;
}

// Permite excluir um contato da agenda
Contact *delContact(Contact *root, char name[30]) {
  if (root == NULL)
    return root;

  if (strcmp(name, root->name) < 0)
    root->left = delContact(root->left, name);

  else if (strcmp(name, root->name) > 0)
    root->right = delContact(root->right, name);

  else {
    if ((root->left == NULL) || (root->right == NULL)) {
      Contact *temp = root->left ? root->left : root->right;

      if (temp == NULL) {
        temp = root;
        root = NULL;
      } else
        *root = *temp;
      free(temp);
    } else {
      Contact *temp = minValueNode(root->right);

      strcpy(root->name, temp->name);

      root->right = delContact(root->right, temp->name);
    }
  }

  if (root == NULL)
    return root;

  // Update no balance de cada node e balanceia
  root->height = 1 + max(height(root->left), height(root->right));

  int balance = getBalance(root);
  if (balance > 1 && getBalance(root->left) >= 0)
    return rightRotate(root);

  if (balance > 1 && getBalance(root->left) < 0) {
    root->left = leftRotate(root->left);
    return rightRotate(root);
  }

  if (balance < -1 && getBalance(root->right) <= 0)
    return leftRotate(root);

  if (balance < -1 && getBalance(root->right) > 0) {
    root->right = rightRotate(root->right);
    return leftRotate(root);
  }

  return root;
}

// Apresenta o menu da aplicação e retorna a opção selecionada
int menu() {
  int op = 0;
  // while (op != EXIT) {
  printf("\n============Contatos do Dr. Chapatin============\n");
  printf("\n[1] Adicionar Contato\n");
  printf("[2] Deletar Contato\n");
  printf("[3] Editar Contato\n");
  printf("[4] Inspecionar Contato\n");
  printf("[5] Listar Contatos\n");
  printf("[%d] Finaliza\n", EXIT);
  printf("\nOpcao: ");
  scanf("%d", &op);
  //}
  return op;
}

// Lista o conteudo da agenda (todos os campos)
// impressão inOrder pelo nome
void listContacts(Contact *root) {

  if (root == NULL) {
    return;
  }

  listContacts(root->left);
  printf("	%s\n", root->name);
  listContacts(root->right);

  return;
}

void loopSaveContacts(Contact *root, FILE *outfile) {

  if (root == NULL) {
    return;
  }

  loopSaveContacts(root->left, outfile);
  fwrite(root, sizeof(Contact), 1, outfile);
  loopSaveContacts(root->right, outfile);

  return;
}
void saveContacts(Contact *root) {
  FILE *file;
  file = fopen("contacts.dat", "w");
  loopSaveContacts(root, file);
  fclose(file);
  return;
}

Contact *loadContacts(Contact *root) {
  FILE *infile;
  Contact input;

  infile = fopen("contacts.dat", "r");
  if (infile == NULL) {
    fprintf(stderr, "\nError opening file\n");
    exit(1);
  }

  while (fread(&input, sizeof(Contact), 1, infile)) {
    root = insContact(root, input.name, input.birth, input.email, input.phone);
  }

  fclose(infile);
  return root;
}

// Permite consultar um contato da agenda por nome
Contact *queryContact(Contact *root, char name[30]) {
  Contact *aux = root;
  while (aux != NULL) {
    if (strcmp(aux->name, name) == 0) {
      break;
    } else if (strcmp(aux->name, name) < 0) {
      aux = aux->right;
    } else if (strcmp(aux->name, name) > 0) {
      aux = aux->left;
    }
  }

  return aux;
}

bool contactExists(Contact *root, char name[30]) {
  if (root == NULL)
    return false;

  else if (strcmp(root->name, name) == 0)
    return true;

  else if (strcmp(root->name, name) > 0) {
    bool val = contactExists(root->left, name);
    return val;
  } else {
    bool val = contactExists(root->right, name);
    return val;
  }
}

void updateBD(Contact *root) {
  remove("contacts.dat");
  saveContacts(root);
  return;
}

void printData(Contact *node) {
  printf("Nome: %s\n", node->name);
  printf("Email: %s\n", node->email);
  printf("Telefone: %s\n", node->phone);
  printf("Data de Nascimento: %d/%d/%d\n", node->birth.day, node->birth.month,
         node->birth.year);
}

// Permite a atualização dos dados de um contato da agenda
Contact *upContact(Contact *root, char name[30]) {
  char newName[30];
  char phone[15];
  Date birth;
  char email[40];
  printf("Novos Dados:\n");
  printf("Nome: ");
  scanf("%s", newName);
  printf("Email: ");
  scanf("%s", email);
  printf("Numero de Telefone: ");
  scanf("%s", phone);
  printf("Data de nascimento:\n");
  printf("Dia: ");
  scanf("%d", &birth.day);
  printf("Mes: ");
  scanf("%d", &birth.month);
  printf("Ano: ");
  scanf("%d", &birth.year);
  root = delContact(root, name);
  root = insContact(root, newName, birth, email, phone);
  updateBD(root);

  return root;
}

// Programa principal
int main() {

  // Cria o arquivo se ele não existir;
  FILE *fp = fopen("contacts.dat", "ab+");
  fclose(fp);

  Contact *root = NULL;
  root = loadContacts(root);
  int op = 0;
  Contact MContact;

  while (op != EXIT) {
    char name[30];
    char phone[15];
    Date date;
    char email[40];
    op = menu();
    switch (op) {
    case 1:
      printf("Nome: ");
      scanf("%s", name);
      printf("Email: ");
      scanf("%s", email);
      printf("Numero de Telefone: ");
      scanf("%s", phone);
      printf("Data de nascimento:\n");
      printf("Dia: ");
      scanf("%d", &date.day);
      printf("Mes: ");
      scanf("%d", &date.month);
      printf("Ano: ");
      scanf("%d", &date.year);
      root = insContact(root, name, date, email, phone);
      updateBD(root);
      break;
    case 2:
      printf("Nome do contato que deseja deletar: ");
      scanf("%s", name);
      if (contactExists(root, name)) {
        root = delContact(root, name);
        updateBD(root);
      } else {
        printf("Nao ha contato com esse nome\n");
      }
      break;
    case 3:
      printf("Nome do contato que deseja alterar: ");
      scanf("%s", name);
      if (contactExists(root, name)) {
        root = upContact(root, name);
      } else {
        printf("Nao ha contato com esse nome\n");
      }
      break;
    case 4:
      printf("Nome do contato que deseja inspecionar: ");
      scanf("%s", name);
      if (contactExists(root, name)) {
        printData(queryContact(root, name));
      } else {
        printf("Nao ha contato com esse nome\n");
      }
      break;
    case 5:
			printf("\n============Contatos do Dr. Chapatin============\n");
      listContacts(root);
      break;
    default:
      if (op != EXIT) {
        printf("Opcao Invailda");
      }
      break;
    }
  }
  return 0;
}
