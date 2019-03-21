#include "../includes/utility.h"

typedef struct User{
    char username[100];
    char password[100];
    char sender[100];
} User;

struct Arguments{
    int fd;
    const char* username;
};

pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

void get_user(User* user, int argv,char** argc);

void *recv_msg_server(void* args);
void *send_msg_server(void* fd);

int main(int argv, char** argc){
    if( argv < 4 || argv > 7){
        eprint("Please provide proper arguments!");
        exit(1);
    }
    
    User user;
    memset(user.password,0,100);
    memset(user.username,0,100);
    get_user(&user,argv,argc);

    struct sockaddr_in client;
    int connection;
    int fd = socket(AF_INET,SOCK_STREAM,0);

    client.sin_family = AF_INET;
    client.sin_port = htons(8096);

    inet_pton(AF_INET, "127.0.0.1", &client.sin_addr);

    connection = connect(fd,(struct sockaddr *)&client, sizeof(client));

    if(connection < 0){
        eprint("Unable to connect to Server!");
        close(fd);
        exit(2);
    }
    char user_info[2048] = {0};
    sprintf(user_info,"%s : %s : %s",user.username,user.password,user.sender);
    
    write(fd,user_info,strlen(user_info));
    memset(user.password,0,100);
    
    pthread_t recv_msg_thread;
    pthread_t send_msg_thread;

    struct Arguments args;
    args.fd = fd;
    args.username = user.username;
    pthread_create(&recv_msg_thread,NULL,recv_msg_server,(void*)&fd);
    pthread_create(&send_msg_thread,NULL,send_msg_server,(void*)&args);

    pthread_join(recv_msg_thread,NULL);
    pthread_join(send_msg_thread,NULL);
    close(fd);
    return 0;
}

void get_user(User* user, int argv,char** argc){
    for(int i = 1; i < argv; i++){
        if(!strcmp(argc[i],"-u")){
            if( argc[i + 1][0] != '-' ){
                strcpy(user->username,argc[++i]);
            }
            else{
                eprint("Please provide proper arguments!");
                exit(1); 
            }
        }

        if(!strcmp(argc[i],"-s")){
            if( argc[i + 1][0] != '-' ){
                strcpy(user->sender,argc[++i]);
            }
            else{
                eprint("Please provide proper arguments!");
                exit(1); 
            }
        }

        if(!strcmp(argc[i],"-p")){
            if( argv == 7 && argc[i + 1][0] != '-' ){
                strcpy(user->password,argc[++i]);
                wprint("Insecure way of typing password!");
            }else if( argv == 4){
                const char* temp = getpass("");
                if(temp == NULL || strlen(temp) == 0) {
                    eprint("Incorrect Password!");
                    exit(1);
                }else{
                    strcpy(user->password,temp);
                }
            }
        }
    }
}

void *recv_msg_server(void* f){
    char server_msg[100];
    int fd = *(int*)f;
    while(1){
        if(read(fd,server_msg,100) <= 0){
            return NULL;
        }
        // pthread_mutex_lock(&mu);
        printf("%s",server_msg);
        // pthread_mutex_unlock(&mu);
    }

    return NULL;
}
void *send_msg_server(void* args){
    char client_msg[100];
    struct Arguments f = *(struct Arguments*)args;

    while(1){
        // char temp_user[100];
        // sprintf(temp_user,"\x1b[1m%s\x1b[0m : ",f.username);
        // write(1,temp_user,strlen(temp_user));
        int ln = read(0,client_msg,100);
        if(ln < 0){
            return NULL;
        }
        write(f.fd,client_msg,ln);
    }
    return NULL;
}