DROP DATABASE IF EXISTS M7_BBDDJuego;
CREATE DATABASE M7_BBDDJuego;

USE M7_BBDDJuego;

CREATE TABLE Jugador(
     usuario VARCHAR(30) NOT NULL,
     contraseña VARCHAR(30) NOT NULL,
     id_usuario INT,
     partidas_ganadas INT,
     PRIMARY KEY(usuario)
)ENGINE=InnoDB;


CREATE TABLE Partida (
    id_partida INT NOT NULL,
    fecha_hora DATE NOT NULL,
    duracion TIME,
    jugador1 VARCHAR(30),
    jugador2 VARCHAR(30),
    ganador VARCHAR(30),		
    PRIMARY KEY (id_partida) 
)ENGINE=InnoDB;

CREATE TABLE Competicion(
     jugador_id VARCHAR(30) NOT NULL,
     partida_id INT NOT NULL,
     ganador VARCHAR(30) NOT NULL,
     puntos INT,
     FOREIGN KEY (jugador_id) REFERENCES Jugador(usuario),
     FOREIGN KEY (partida_id) REFERENCES Partida(id_partida),
     PRIMARY KEY(jugador_id,partida_id)
)ENGINE=InnoDB;

INSERT INTO Jugador VALUES ('Dorian', '1610lol', 1, 4);  
INSERT INTO Jugador VALUES ('Claudia', 'clau666', 2, 5); 
INSERT INTO Jugador VALUES ('Maria', '3456op', 3, 1);
INSERT INTO Jugador VALUES ('Valeria', 'vale879', 4, 5);
INSERT INTO Jugador VALUES ('Pedro', '1945sg', 6, 2); 
INSERT INTO Jugador VALUES ('Jose', '4561hg', 7, 8);

INSERT INTO Partida VALUES (1, '2023-12-04 19:00:23', '00:30:00', 'Dorian', 'Pedro', 'Pedro');
INSERT INTO Partida VALUES (2, '2014-04.23 12:08:00', '00:45:00','Claudia','Jose', 'Jose');
INSERT INTO Partida VALUES (3, '2019-09-12 09:56:01', '00:20:00', 'Valeria', 'Maria', 'Maria');
INSERT INTO Partida VALUES (4, '2019-09-12 09:56:01', '00:30:00', 'Dorian', 'Pedro', 'Dorian');
INSERT INTO Partida VALUES (5, '2019-09-12 09:56:01', '00:50:00', 'Valeria', 'Dorian', 'Valeria');





