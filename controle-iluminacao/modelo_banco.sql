CREATE DATABASE lap; 

CREATE TABLE luminarias
(
	id int NOT NULL,
	status int NOT NULL DEFAULT 0,
	pot int NOT NULL DEFAULT 0,	
    PRIMARY KEY (id)
);

INSERT INTO luminarias(id, status, pot) VALUES(1, 0, 0);
INSERT INTO luminarias(id, status, pot) VALUES(2, 0, 0);
INSERT INTO luminarias(id, status, pot) VALUES(3, 0, 0);
