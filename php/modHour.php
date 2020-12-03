<?php
    date_default_timezone_set('America/Mexico_City');
$password = $_POST['key'];
if($password == "PASSWORD"){
    $host= "host";
    $user= "username";
    $dbname= "dbname";
    
    $id = $_POST['id'];
    $nextDose = $_POST['nextDose'];
 
    $conexion = mysqli_connect($host, $user, $password, $dbname);
    
    $sql = "UPDATE `medNames` SET nextDose = '$nextDose' WHERE `medNames`.`tubeNum` = $id";
    
    
    if(mysqli_query($conexion, $sql))
        echo $sql;
        





}





?>
