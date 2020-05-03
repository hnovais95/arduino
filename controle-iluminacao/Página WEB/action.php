<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>Atualizando...</title>
<link rel="icon" href="favicon.ico" type="image/x-icon" />
<link rel="shortcut icon" href="favicon.ico" type="image/x-icon" />
</head>
<body>
<?php 

//Dados de conexão:
$servername = "localhost";
$username = "heitor";
$password = "segredo";
$dbname = "lap"; 

//Conectando com o localhost - mysql
$conn = new mysqli($servername, $username, $password, $dbname);

//Verificando conexão
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

//Recebendo dados preenchidos no formulário
$id = $_POST ["id"];			//atribuição do campo "id" vindo do formulário para variavel	
$pot = $_POST ["pot"];			//atribuição do campo "pot" vindo do formulário para variavel
$status = $_POST ["status"];	//atribuição do campo "status" vindo do formulário para variavel

//Gravando dados no banco de dados
$sql = //"INSERT INTO `luminarias` ( `id` , `pot`, `status` ) VALUES ('$id', '$pot','$status')";
		"UPDATE luminarias
		SET pot = $pot,
			status = $status
		WHERE id = $id
		";

if ($conn->query($sql) === TRUE) {
    //echo "Gravação realizada com sucesso.";
    header('Location: pagina_teste.html'); //Redirecionamento direto após preenchimento do formulários
} else {
    $texto_erro = "Erro: Gravação não realizada. \n\n";
    echo nl2br($texto_erro) . $conn->error;
}


$conn->close();
?> 
</body>
</html>