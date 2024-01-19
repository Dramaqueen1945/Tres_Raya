#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <pthread.h>

//------------VARIABLES GLOBALES 
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
int sockets[100];
int contador;
int i;
MYSQL *conn;

//-----------ESTRUCTURAS DE DATOS

//Funciones que haremos servir para lo que es darse de alta y lista de conectados 
typedef struct {
	
	char nombre[20];
	int socket;
}Jugador;   //Cada jugador tendra su propio socket y su nombre como identificador

typedef struct{
	
	Jugador conectados[100];
	int cont;
}Conectados; //Creamos una lista de usuarios conectados

Conectados lista;
//Estructuras para invitar a usuarios 

typedef struct{
	char anfitrion[30]; //jugador anfitrion
	char invitado[30];  //jugador invitado
	int id; //identificador de la partida 
	int numjugadores; //el maximo de jugadores ha de ser de dos
	char ganador[30]; //El ganador de la partida
	
}Partida; //Estrcutura de partidas

typedef struct{
	
	Partida partidas[4]; //tenemos como un maximo de 20 partidas
	int num; //El numero de partidas que se estan dando en aquel momento
}ListaPartidas;


//----------FUNCIONES
int NuevoUsuario(char nombre[30], char contra[30]){
	//Variables con las cuales trabajamos con base de datos	
		MYSQL_RES *resultado;
		MYSQL_ROW row;
		int err;
		
		char query[200];
		sprintf(query, "INSERT INTO Jugador (usuario, contraseña, id_usuario, partidas_ganadas) VALUES ('%s','%s', 0, 0)", nombre, contra);
		printf("%s\n", query);
		
		// Ejecutar la consulta
		err = mysql_query(conn, query);
		// Manejar errores
		if (err != 0) {
			fprintf(stderr, "Error al ejecutar la consulta: %s\n", mysql_error(conn));
			
			return -1;
		}
		
		return 1; //Exito
	exit(0);
}

int Registrar(char nombre[30], char contra[30]){
	//Variables con las cuales trabajamos con base de datos	
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	int err;
	char query[200];
	//Hacemos que el usuario pueda registrarse mediante una peticion a la base de datos 
	sprintf(query,"SELECT Jugador.usuario, Jugador.contraseña FROM Jugador WHERE Jugador.usuario='%s' AND Jugador.contraseña='%s'", nombre, contra);
	printf("%s\n",query);
	err = mysql_query(conn, query);
	
	if(err != 0)
	{
		return -1;
	}
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	
	if (row == NULL)
	{
		return -1;
	}
	else
	{
		printf("Usuario: %s\n", row[0]);
		strcpy(nombre, row[0]);
		printf("Contraseña: %s\n", row[1]);
		row = mysql_fetch_row(resultado);
		return 1;
	}
	exit(0);
}

int PonConectado(char nombre[20], int socket) {
	//A?ade nuevo conectados. retorna 0 si ok y -1 si la lista ya estaba llena y no lo ha podido a?adir
	if (lista.cont == 100)
		return -1;
	else {
		strcpy(lista.conectados[lista.cont].nombre, nombre);
		lista.conectados[lista.cont].socket = socket;
		lista.cont++;
		return 0;
	}
}



void DameConectados(char jugadoresconectados[100])
{
	memset(jugadoresconectados, 0, 100);
	jugadoresconectados[0]= '\0';
	for (int i = 0; i <= lista.cont; i++)
	{
		sprintf(jugadoresconectados, "%s%s,", jugadoresconectados, lista.conectados[i].nombre);
	}
    //Añadir un deiimitador al final 
	//strcat(conectados, "/");
}
void QuitarConectado(char nombre[30]) {
	int i = 0;
	int encontrado = 0;
	
	while ((i < lista.cont) && (!encontrado)) {
		if (strcmp(lista.conectados[i].nombre, nombre) == 0) {
			encontrado = 1;
		} else {
			encontrado = 0;
		}
		
		i++;
	}
	if (encontrado == 1) {
		// Disminuimos el contador antes de realizar modificaciones
		lista.cont--;
		
		// Eliminamos el usuario de la lista de conectados
		lista.conectados[i - 1].socket = 0;
		strcpy(lista.conectados[i - 1].nombre, "");
		
		// Desplazamos los usuarios restantes para llenar el espacio vacÃ­o
		for (int j = i - 1; j < lista.cont; j++) {
			lista.conectados[j] = lista.conectados[j + 1];
		}
	}
	
	
}


//Consultas e implementancion de metodos para la invitacion 

//---------Funciones para crear la partida o crear el juego 

ListaPartidas partidas;

void InvitarJugador(char anfitrion[30], char invitado[30])
{
	int i = 0;
	int encontrado = 0;
	char respuesta[30];
	while (i < lista.cont && encontrado == 0)
	{
		if (strcmp(lista.conectados[i].nombre, invitado) == 0)
		{
			encontrado = 1;
		}
		else
			i++;
	}
	if (encontrado == 1)
	{
		// write(sock_conn, respuesta, strlen(respuesta));
		sprintf(respuesta, "7/%s\n", anfitrion);
		write(lista.conectados[i].socket, respuesta, strlen(respuesta));
		
	}
	
}

int CrearPartida(char invitado[30], char anfitrion[30])
{
	
	//Generamos un numero aleatorio para el numero de identificador de la partida
	srand(time(NULL));
	int random= rand() % 100 + 1;
	
	if(partidas.num == 4) //tenemos que el numero de partidas esta lleno 
	{
		return -1;
	}
	
	else  //Tenemos espacio suficiente para poder crear una nueva partida  
	{
		int i= partidas.num;
		partidas.partidas[i].id= random;
		strcpy(partidas.partidas[i].anfitrion,anfitrion);
		strcpy(partidas.partidas[i].invitado,invitado);
		partidas.num= partidas.num+1;
		printf("Anfitrion: %s\n", partidas.partidas[i].anfitrion);
		printf("Invitado: %s\n", partidas.partidas[i].invitado);
		printf("Numero de partida: %d\n", partidas.partidas[i].id);
		printf("Se ha podido crear correctamente la partida y tenemos %d partidas\n", partidas.num);
		return 0;		
	}
	
}

//---Funciones de cara a lo que vienen a ser las consultas del jugador 

int Consulta1 (char jugador[30], char oponentes[100])  //Se nos da una lista con los jugadores con los que me he echado una partida 
{
	//Abremos de hacer un recorrido para ver todos los usuarios con los cuales ha jugado una partida el cliente 
	MYSQL_RES* resultado;
	MYSQL_ROW row;
	int err;
	char jugadores[100];
	char consulta[200];
	sprintf(consulta, "SELECT GROUP_CONCAT(CASE WHEN Partida.jugador1 = '%s' THEN Partida.jugador2 ELSE Partida.jugador1 END SEPARATOR ',') FROM Partida WHERE Partida.jugador1 = '%s' OR Partida.jugador2 = '%s'", jugador, jugador, jugador);
	
	printf("%s\n", consulta);
	err = mysql_query(conn,consulta);
	
	if (err != 0)
	{
		printf("Error al consultar datos de la base %u %s\n",
			   mysql_errno(conn), mysql_error(conn));
		exit(1);
		return -1;
	}
	
	resultado = mysql_store_result(conn);
	row = mysql_fetch_row(resultado);
	
	if (row == NULL) {
		printf("No se han obtenido datos en la consulta\n");
		return -2;
	} else {
		// La columna 0 contiene la cadena de jugadores separada por '/' incluyendo el jugador obtenido por parametro 
		strcpy(oponentes, row[0]); 
		printf("Jugadores: %s\n", oponentes);
		return 0;
	}
	
	mysql_close(conn);
	exit(0);
}

int Consulta2(char oponente[30], char nombre[30], char ganador[30])
{
	MYSQL_RES* resultado;
	MYSQL_ROW row;
	int err;
	int cont = 1;
	
	char query[200];
	sprintf(query, "SELECT Partida.ganador FROM Partida WHERE (Partida.jugador1 = '%s' AND Partida.jugador2= '%s') OR (Partida.jugador1 = '%s' AND Partida.jugador2 = '%s')", oponente, nombre, nombre, oponente);
	printf("%s\n", query);
	err = mysql_query(conn, query);
	
	
	if (err != 0)
	{
		printf("Error al consultar datos de la base\n");
		return -1;
	}
	
	resultado = mysql_store_result(conn);
	row = mysql_fetch_row(resultado);
	
	if (row == NULL)
	{
		printf("No se han obtenido datos en la consulta\n");
		return -2;
	}
	
	else
	{
		
		while (row != NULL) {
			
			printf("Ganador: %s\n", row[0]);
			strcpy(ganador, row[0]);
;			row = mysql_fetch_row(resultado);
			cont++;
		}
		return 0;
	}
	
	mysql_close(conn);
	exit(0);
}

int Consulta3 () //La que nos falta por agregar, si tenemos tiempo la implementamos 
{
	
}

int DameSocket(char nombre[30]) {
	int encontrado = 0;
	int i = 0;
	
	while (i < lista.cont && !encontrado) {
		if (strcmp(lista.conectados[i].nombre, nombre) == 0) {
			encontrado = 1;
		} else {
			i++;
		}
	}
	
	if (encontrado) {
		return lista.conectados[i].socket;
	} else {
		return -1;
	}
}
 void ImprimeConectados()
 {
	 for(int i=0; i< lista.cont; i++)
	 {
		 printf("Conectados: %s\n", lista.conectados[i].nombre);
	 }
 }
 

//------------------------FUNCION ATENDER CLIENTE--------------------------------

void *AtenderCliente(void *socket)
{
	int sock_conn;
	char buffer[512];
	char respuesta[512];
	char notificacion[512];
	int ret;
	int *s;
	char conectados[100];
	
	s = (int*)socket;
	sock_conn= *s;
	
	//Empezamos con el bucle infinito con el cual realizaremos las peticiones del cliente 
	
	int terminar=0;
	while(terminar == 0)
	{
		ret= read(sock_conn, buffer, sizeof(buffer));
		printf ("Recibido\n");
		buffer[ret]= '\0';
		printf ("Peticion: %s\n",buffer);
		
		//Vemos que tipo de peticion es 
		//Con el puntero cogemos primero el numero de peticion hecha por el cliente 
		char *p = strtok(buffer, "/");
		int codigo= atoi(p);
		char nombre[30];
		char contra[30];
		char anfitrion[30];
		char invitado[30];
		
		switch(codigo){
		
			
		case 1:  //Creamos un nuevo usuario 
		{
		//Recuperamos los valores de nombre y contraseña del nuevo usuario 
		  p = strtok(NULL, "/");
		  strcpy(nombre,p);
		  p = strtok(NULL, "/");
		  strcpy(contra,p);
		
		  printf ("Nombre: %s\n, Contraseña: %s\n", nombre,contra);
		  int err = NuevoUsuario(nombre, contra);
		  
		  if(err == -1)
		  {
			  sprintf (respuesta, "1/0");
			
		  }
		  
		  else if (err == 1)
		  {
			  sprintf(respuesta, "1/1");
			
		  }
		  
		  printf("----------------------------------\n");
		break;
		}
		
		case 2: //Damos de alta a un usuario 
		{
		//Recuperamos los datos de usuario 
			p = strtok( NULL, "/");
			strcpy (nombre, p);
			p = strtok( NULL, "/");
			strcpy (contra, p);
			printf ("Nombre: %s\n, Contraseña: %s\n", nombre,contra);
			int res = Registrar(nombre, contra);
			if(res == 1)
			{
				sprintf(respuesta, "2/1");   //se ha registrado correctamente
				pthread_mutex_unlock( &mutex );
				PonConectado(nombre, sock_conn);
				pthread_mutex_unlock( &mutex);
				
			
			}
			
			//Cada vez que se loguea alguien tambien hemos de actualizar la lista de conectados y darsela a todos los clientes que se encuentran conectados 
			
			else
			{
				sprintf(respuesta, "2/0");  //contraseña o usuario incorrecto 
	           
			}
			
			
			printf("----------------------------------\n");
			
		break;
		}
	
		case 3: //Consulta1: Devolvemos una lista con los nombres de los usuarios con los que hemos jugado anteriormente 
		{
			//Obtenemos el nombre del usuario 
			p= strtok(NULL, "/");
			strcpy(nombre, p);
			char jugadores[100];
			int err= Consulta1(nombre, jugadores);
			
			if(err==-1)
			{
			    sprintf(respuesta, "3/1"); //Error al consultar la base de datos 	
			}
			else if(err==2)
			{
				sprintf(respuesta, "3/2"); //No se ha obtenido datos en la consulta 
			}
			else if(err==0)
			{
				sprintf(respuesta, "3/0/%s", jugadores); //Nos pasa el vector con los nombres de los usuarios que he hecho una partida
				
			}
		  break;
		}
		
		case 4: //Consulta2: Devolvemos los resultados obtenido con el jugador dado por el cliente 
		{
			char opo[30];
			p= strtok(NULL, "/");    //4/oponente/cliente
			strcpy(opo,p);
			p= strtok(NULL, "/");
			strcpy(nombre,p);
			
			char ganador[30];
			int err= Consulta2(opo, nombre, ganador);
			if(err==-1)
			{
			     sprintf(respuesta, "4/1")	;  //Error al consultar la base de datos 
			}
			if(err==-2)
			{
				sprintf(respuesta, "4/2"); //no se ha encontrado ninguna partida en la que juegues con este oponente
			}
			
			if(err==0)
			{
			    sprintf(respuesta, "4/0/%s", ganador);	//Devolvemos el nombre con el jugador ganador 
			}
		   break;
		}
		case 5: //Consulta3: Devolvemos una lista con las partidas de una determinada duracion 
		{
		  
			break;
		}
		
		case 6: //Devolvemos la lista con los usuarios conectados 
		{	conn = mysql_init(NULL);
		if (conn == NULL)
		{
			printf("Error al crear la conexion: %u %s\n",
				   mysql_errno(conn), mysql_error(conn));
			exit(1);
		}
		
		conn = mysql_real_connect(conn, "localhost", "root", "mysql", "M7_BBDDJuego", 0, NULL, 0);
		
		if (conn == NULL)
		{
			printf("Error al inicializar la conexion: %u %s\n",
				   mysql_errno(conn), mysql_error(conn));
			exit(1);
		}
		
		
		DameConectados(conectados);
		printf("Resultado conectados: %s\n", conectados);  //Tendriamos que obtener un vector con los nombre separados por coma 
		sprintf(respuesta, "6/%s", conectados); 
		printf("Respuesta: %s\n", respuesta);
		for (int i = 0; i <= lista.cont; i++)     //Damo lista de conectados a todos los conectados al servidor 
		{
			printf("Conectados a los que enviamos: %s\n", lista.conectados[i].nombre);
			write(lista.conectados[i].socket, respuesta, strlen(respuesta));
		}
		break;
		}
		
		
		case 7: //Realizamos invitacion a un jugador 
		{	

			p = strtok(NULL, "/"); //Recibo: /anfitrion/invitado
			strcpy(anfitrion,p); 
			p= strtok(NULL, "/");
			strcpy(invitado,p);
			char invitado[30];
			strcpy(invitado, p);
			InvitarJugador(anfitrion, invitado);
		  break;
		}
		case 8: //Aceptamos o rechazamos invitacion 
		{
		
			// Recibimos la contestacion del jugador a nuestra invitacion
			p = strtok(NULL, "/");   //Inicialmente recibimos el formato 8/invitado/anfitrion
			int contestacion = atoi(p);
			//El jugador ha aceptado nuestra invitacion
			if (contestacion == 0) 
			{
				p = strtok(NULL, "/");
				strcpy(anfitrion,p);
				p= strtok(NULL, "/");
				strcpy(invitado,p);
				int e = CrearPartida(invitado, anfitrion);
				if (e == 0)
				{
					sprintf(respuesta, "8/%s", invitado); // Se acepta la solicitud de juego
					printf("%s\n", respuesta);
					write(sock_conn, respuesta, strlen(respuesta));
					
				}
				
				else
					sprintf(respuesta, "9/0"); // No hay espacio para mï¿¡s jugadpres
			}
			if (contestacion == 1)    //No nos ha aceptado la invitacion 
			{
				sprintf(respuesta, "9/1"); // No se acepta la solicitud de juego
				printf("%s\n", respuesta);
			}
		  break;
		}
		
		case 0: //Desconexion con el servidor  
		{
			
			p= strtok(NULL,"/");
			strcpy(nombre,p);
			QuitarConectado(nombre);
			sprintf(respuesta, "0/\n");
	
			
			
			//Una vez el usuario se ha cerrado sesion hemos de notificar de esto a todos los usuarios aun conectados 
			printf("Resultado conectados: %s\n", conectados); 
			sprintf(notificacion, "12/%s\n", conectados); 
			for (int i = 0; i <= lista.cont; i++)
			{
				printf("Conectados a los que enviamos: %s\n", lista.conectados[i].nombre);
				write(lista.conectados[i].socket, notificacion, strlen(notificacion));
			}
			
			terminar = 1;
			break;
		}
		
		case 9: //Cerramos sesion 
		{
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			QuitarConectado(nombre);
			sprintf(respuesta, "10/");
			// Notificamos a todos los usuarios conectados sobre la desconexion
			sprintf(notificacion, "10/%s", conectados);
			// Corregir la condiciÃ³n del bucle para evitar acceder a un elemento no inicializado
			for (int i = 0; i < lista.cont; i++)
			{
				printf("Conectados a los que enviamos: %s\n", lista.conectados[i].nombre);
				
				// Verificar que el socket sea vÃ¡lido antes de enviar la notificaciÃ³n
				if (lista.conectados[i].socket > 0) {
					write(lista.conectados[i].socket, notificacion, strlen(notificacion));
				}
			}
			
		  break;
		}
		
		case 14: //Nos guarda todo lo que vienen a ser las casillas 
		{//RECIBO   14/codigoboton/oponente		envio 20/codigoboton(xotrosoket)
		    char boton[20];
			p= strtok(NULL,"/");
			strcpy(boton,p);
			p= strtok(NULL, "/");
			strcpy(anfitrion,p);
			ImprimeConectados();
			pthread_mutex_lock(&mutex);
			int err= DameSocket(anfitrion);
			pthread_mutex_unlock(&mutex);
			if(err==-1)
			{
				sprintf(respuesta,"14/No se ha encontrado a la persona");
				printf("Respuesta: %s\n",respuesta);
				write(sock_conn,respuesta,strlen(respuesta));  //Envaimos al usuario anfitrion que no se ha encontrado al oponente
			}
			
			else{
				//envíamos el nombre de la persona que está invitando
				sprintf(respuesta,"14/Es tu turno"); 
				write(err,respuesta,strlen(respuesta)); 
				
			}
			break;
		}
		
		case 15:    //Guardamos los datos de la partida
		{
			p= strtok(NULL,"/");
			char ganador[20];
			//int= ActualizamosPartida(ganador);
			
			
			
		     break;
		}
		case 10: //Eliminamos al usuario que d¡se quiere de baja de la base de datos 
		{
		    //
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			MYSQL_RES* resultado;
			MYSQL_ROW row;
			int err;
			int cont = 1;
			char respuesta[30];
			
			// Eliminamos el usuario de las estructuras de datos
			pthread_mutex_lock(&mutex);
			QuitarConectado(nombre);
			pthread_mutex_unlock(&mutex);
			// Eliminamos el usuario de la base de datos
			char query[200];
			sprintf(query, "DELETE FROM Jugador WHERE usuario = '%s'", nombre);
			printf("%s\n", query);
			err = mysql_query(conn, query);
			
			if (err != 0)
			{
				printf("Error al consultar datos de la base\n");
				sprintf(respuesta, "11/0"); // Error al consultar la base de datos
			}
			else
			{
				printf("Usuario eliminado correctamente");
				sprintf(respuesta, "11/1"); // Se ha eliminado con Ã©xito al usuario
			}
			
		}
		
		default:
			break;
		}
		
		if (codigo != 0 && codigo != 6 && codigo != 12 && codigo != 7 && codigo != 14 && codigo != 8)
		{ // Enviamos la respuesta al servidor
			printf("%s\n", respuesta);
			write(sock_conn, respuesta, strlen(respuesta));
			printf("resultado: %s\n", conectados);
		}
		if (codigo == 0)
		{
			printf("%s\n", respuesta);
			write(sock_conn, respuesta, strlen(respuesta));
		}
	
	
	
	}
		
	}




int main(int argc, char *argv[]) {
	
	//Usamos el main para la conexion con la base de datos y el servidor 
	
	int client_socket;
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		
		printf("Error creant socket");
		exit(EXIT_FAILURE);	
	}
	// Fem el bind al port
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9084);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	//Conexion MYSQL
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit;
	}
	
	conn = mysql_real_connect (conn, "localhost", "root", "mysql", "M7_BBDDJuego", 0, NULL, 0);
	
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit;
	}
	contador =0;
	pthread_t thread;
	i=0;
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		sockets[i] =sock_conn;
		
		//sock_conn es el socket que usaremos para este cliente
		// Crear thead y decirle lo que tiene que hacer
		
		pthread_create (&thread, NULL, AtenderCliente,&sockets[i]);
		i=i+1;
		
	}
	
	mysql_close(conn);
	exit(0);
}

