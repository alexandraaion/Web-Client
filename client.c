#include <stdlib.h>     
#include <stdio.h>
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <string.h>
#include "cJSON.h"


int main(int argc, char *argv[])
{
    char comm[20];
    char pass[20], user[20];
    char *title = (char *)malloc(20*sizeof(char));
    char *author = (char *)malloc(20*sizeof(char));
    char *genre = (char *)malloc(20*sizeof(char));
    char *publisher = (char *)malloc(20*sizeof(char));
    char *cookie = (char *)malloc(80*sizeof(char));
    char *auxC = (char *)malloc(80*sizeof(char));
    char *token = NULL;
    char *response, *message, *string, *pointer1, *pointer2;
    cJSON *tokenAux;
    int page_count;
    int id;
    int sockfd;

    while(1) {
        printf("\n");
        //citim de la tastatura o comanda
        scanf("%s", comm);

        if (strcmp(comm, "register") == 0) {
            printf("username=");
            scanf("%s", user);
            printf("password=");
            scanf("%s", pass);

            //cream payload-ul de tipul json cu user-ul si parola
            cJSON *json = cJSON_CreateObject();
            cJSON_AddStringToObject(json, "username", user);
            cJSON_AddStringToObject(json, "password", pass);
            string = cJSON_Print(json);
            
            //deschidem o noua conexiune si trimitem un request de tip post
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request("3.8.116.10", 
                        "/api/v1/tema/auth/register", "application/json", 
                        &string, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("%s\n", response);

            close_connection(sockfd);
        }else if (strcmp(comm, "login") == 0) {
            printf("username=");
            scanf("%s", user);
            printf("password=");
            scanf("%s", pass);

            //cream payload-ul de tipul json cu user-ul si parola
            cJSON *json = cJSON_CreateObject();
            cJSON_AddStringToObject(json, "username", user);
            cJSON_AddStringToObject(json, "password", pass);
            string = cJSON_Print(json);
            
            //deschidem o noua conexiune si trimitem un request de tip post
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 
                                    0);
            message = compute_post_request("3.8.116.10", 
                    "/api/v1/tema/auth/login", "application/json", &string, 1, 
                    NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            //extragem cookie-ul din raspuns
            pointer1 = strstr(response, "Set-Cookie:");

            if (pointer1 != NULL) {
                strcpy(auxC, pointer1 + strlen("Set-Cookie: "));
                cookie = strtok(auxC, ";");
            }
            printf("%s\n", response);

            close_connection(sockfd);
        }else if (strcmp(comm, "enter_library") == 0) {
            //deschidem o noua conexiune si trimitem un request de tip get
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 
                                    0);
            message = compute_get_request("3.8.116.10", 
                    "/api/v1/tema/library/access", NULL, &cookie, 1, NULL);
            printf("\n");
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            //extragem token-ul JWT din raspuns
            pointer2 = strchr(response, '{');
            if (pointer2 != NULL){
                cJSON *parsedData = cJSON_Parse(pointer2);
                tokenAux = cJSON_GetObjectItemCaseSensitive(parsedData, "token");
                if (tokenAux != NULL)
                    token = tokenAux->valuestring;
            }
            printf("%s\n", response);

            close_connection(sockfd);
        } else if (strcmp(comm, "get_books") == 0) {
            //deschidem o noua conexiune si trimitem un request de tip get
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("3.8.116.10", 
                    "/api/v1/tema/library/books", NULL, NULL, 0, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("%s\n", response);
            
            close_connection(sockfd);
        } else if (strcmp(comm, "get_book") == 0) {
            //citim id-ul cartii de la tastatura
            printf("id=");
            scanf("%d", &id);
            char aux[50];
            char i[5];
            //cream ruta de acces pentru cererea de tip get
            strcpy(aux, "/api/v1/tema/library/books/");
            sprintf(i, "%d", id);
            strcat(aux, i);

            //deschidem o noua conexiune si trimitem un request de tip get
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("3.8.116.10", aux, NULL, NULL, 0, 
                    token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("%s\n", response);
            
            close_connection(sockfd);
        } else if (strcmp(comm, "add_book") == 0) {
            //citim de la tastatura detaliile necesare
            printf("title=");
            scanf("%s", title);
            printf("author=");
            scanf("%s", author);
            printf("genre=");
            scanf("%s", genre);
            printf("publisher=");
            scanf("%s", publisher);
            printf("page_count=");
            scanf("%d", &page_count);

            //cream payload-ul de tipul json cu informatiile de la tastatura
            cJSON *json2 = cJSON_CreateObject();
            cJSON_AddStringToObject(json2, "title", title);
            cJSON_AddStringToObject(json2, "author", author);
            cJSON_AddStringToObject(json2, "genre", genre);
            cJSON_AddNumberToObject(json2, "page_count", page_count);
            cJSON_AddStringToObject(json2, "publisher", publisher);
            string = cJSON_Print(json2);

            //deschidem o noua conexiune si trimitem un request de tip post
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 
                    0);
            message = compute_post_request("3.8.116.10", 
                    "/api/v1/tema/library/books", "application/json", &string, 
                    1, NULL, 0, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            printf("%s\n", response);

            close_connection(sockfd);
        } else if (strcmp(comm, "delete_book") == 0) {
            //citim id-ul cartii de la tastatura
            printf("id=");
            scanf("%d", &id);
            char aux[50];
            char i[5];
            //cream ruta de acces pentru cererea de tip delete
            strcpy(aux, "/api/v1/tema/library/books/");
            sprintf(i, "%d", id);
            strcat(aux, i);

            //deschidem o noua conexiune si trimitem un request de tip delete
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 
                    0);
            message = compute_delete_request("3.8.116.10", aux, NULL, NULL, 0, 
                    NULL, 0, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("%s\n", response);
            
            close_connection(sockfd);
        } else if (strcmp(comm, "logout") == 0) {
            //deschidem o noua conexiune si trimitem un request de tip get
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 
                    0);
            message = compute_get_request("3.8.116.10", 
                    "/api/v1/tema/auth/logout", NULL, &cookie, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("%s\n", response);
            
            close_connection(sockfd);
            token = NULL;
            free(cookie);

            continue;
        } else if (strcmp(comm, "exit") == 0) {
            printf("Bye!\n");
            break;
        } else {
            printf("Invalid command. Please try again.\n");
            continue;
        }
            free(message);
            free(response);
    }

    return 0;
}
