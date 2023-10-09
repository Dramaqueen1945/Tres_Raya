DROP DATABASE IF EXISTS Tres_Raya;
CREATE DATABASE Tres_Raya;

USE DATABASE Tres_Raya;

CREATE TABLE Jugador(
     usuario VARCHAR(20) NOT NULL,
     contraseña VARCHAR(30) NOT NULL,
     id_usuario INT,
     PRIMARY KEY(usuario)
}ENGINE = InnoDB;

CREATE TABLE Partida(
     id_partida INT,
     fecha_hora DATETIME,
     duracion TIME,
     Ganador VARCHAR(20),
     PRIMARY KEY(id_partida)
)ENGINE = InnoDB;

CREATE TABLE Tabla(
      id_partida INT,
      id_jugador1 INT,
      id_jugador2 INT,
      FOREIGN KEY(id_partida) REFERENCES Partida(id_partida),
      FOREIGN KEY(id_usuario) REFERENCES Jugador(id_usuario)     
)ENGINE = InnoDB;

INSERT INTO Jugador VALUES ('Dorian', '1610lol', 1);    
INSERT INTO Jugador VALUES ('Jose', '4561hg', 9);
INSERT INTO Jugador VALUES ('Maria', '3456op', 1);
INSERT INTO Jugador VALUES ('Pedro', '1945sg', 5);
INSERT INTO Jugador VALUES ('Valeria', 'vale879', 4);
INSERT INTO Jugador VALUES ('Claudia', 'clau777', 4);

INSERT INTO Partidas VALUES (1, '2023-12-04 19:00:23', '00:30:00', 3, 'Pedro');
INSERT INTO Partidas VALUES (2, '2014-04.23 12:08:00', '00:45:00', 3, 'Dorian');
INSERT INTO Partidas VALUES (3, '2019-09-12 09:56:01', '00:20:00', 2, 'Valeria');
INSERT INTO Partidas VALUES (4, '2003-01-01 04:23:12', '01:09:00', 2, 'Jose');

