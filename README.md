# Telematics Pong 🏓
## Introduction 📄
Pong es un juego clásico de computadoras en que dos o más jugadores se conectan por medio de internet a la aplicación de arquitectura cliente/servidor que soporta el juego. 
En este proyecto se encontrarán implementados conceptos de programación en red, un nuevo protocolo creado, y usando la API de Sockets Berkeley para que la aplicación sea concurrente.
### Team Members 🧑🏻‍💻
- [Miguel Angel Cabrera](https://github.com/Miguelco23)
- [Valeria Guerra Zapata](https://github.com/vguerraz)
- [Angel David Martinez](https://github.com/Angel2327)

## Development
El juego Pong se desarrolló bajo una arquitectura Cliente/Servidor. Donde el cliente recibe la entrada del usuario, y notifica al servidor, que le notifica al otro usuario de la partida. En la estructura del proyecto encontramos dos carpetas: client y server. En la carpeta Client encontramos los archivos constants.py (donde hemos declarado el puerto, la dirección IP del servidor y los comandos definidoos en el protocolo) y pongClient.py. En la carpeta Server encontramos el archivo pongServer.c.

En este juego, cuando ingresa el usuario, se le pide su nombre y se procede a hacer la conexión entre el cliente (que se está ejecutando en el local) y el servidor (que se ejecuta en AWS). Una vez se logra la conexión, se envía un mensaje al servidor indicandole la posición inicial de la pelota, y el servidor responde con la dirección de movimiento que deberá tener la pelota al iniciar el juego (que se elige de forma aleatoria en el servidor con la función "choose"). Luego de esto, el cliente espera que el servidor le de la indicación de iniciar el juego (esto se dará cuando hayan dos jugadores listos en la partida).

En ese momento, se abre la ventana del juego Pong e inicia la partida. La partida termina bajo dos condiciones, la primera es que se cierre la ventana, y la segunda es que alguno de los dos jugadores alcance 10 puntos, y ahí se declara ganador de la partida. 

Para este proyecto se dessarrolló un nuevo protocolo, que está especificado en el documento PDF (Protocolo Telepong - Grupo_ Angel Martinez, Valeria Guerra, Miguel Cabrera.pdf) de esta carpeta. De igual forma se ha realizado un diagrama de secuencia de dicho protocolo (Diagrama Protocolo Telepong.pdf), que se encuentra en PDF en la carpeta del proyecto con el nombre específicado en el paréntesis.

## Conclusions


## References
https://www.youtube.com/watch?v=U28svzb1WUs&t
https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
