<?php
    date_default_timezone_set('America/Mexico_City');
$password = $_POST['key'];

if($password == "PASSWORD"){
    $host= "host";
    $user= "username";
    $dbname= "dbname";
    
    $reg = $_POST['reg'];
    $conexion = mysqli_connect($host, $user, $password, $dbname);
    
    if($reg==1){
        $medicine = $_POST['nameMed1'];
    
        $sql= "SELECT * FROM `medLog` WHERE medicine='$medicine' ORDER BY id DESC LIMIT 1";
        $result = mysqli_query($conexion, $sql);
        $row = mysqli_fetch_assoc($result);
        $id = $row["id"];
     
        $sql = "UPDATE `medLog` SET collected = '1' WHERE `medLog`.`id` = $id";
    
        if(mysqli_query($conexion, $sql))
            echo "OK";
           
        }
        
     else if($reg==2){
        $medicine = $_POST['nameMed2'];
    
        $sql= "SELECT * FROM `medLog` WHERE medicine='$medicine' ORDER BY id DESC LIMIT 1";
        $result = mysqli_query($conexion, $sql);
        $row = mysqli_fetch_assoc($result);
        $id = $row["id"];
     
        $sql = "UPDATE `medLog` SET collected = '1' WHERE `medLog`.`id` = $id";
    
        if(mysqli_query($conexion, $sql))
            echo "OK";
           
        }

     else if($reg==3){
        $medicine = $_POST['nameMed1'];
    
        $sql= "SELECT * FROM `medLog` WHERE medicine='$medicine' ORDER BY id DESC LIMIT 1";
        $result = mysqli_query($conexion, $sql);
        $row = mysqli_fetch_assoc($result);
        $id = $row["id"];
     
        $sql = "UPDATE `medLog` SET collected = '1' WHERE `medLog`.`id` = $id";
    
        mysqli_query($conexion, $sql) or die;
            
        $medicine = $_POST['nameMed2'];
    
        $sql= "SELECT * FROM `medLog` WHERE medicine='$medicine' ORDER BY id DESC LIMIT 1";
        $result = mysqli_query($conexion, $sql);
        $row = mysqli_fetch_assoc($result);
        $id = $row["id"];
     
        $sql = "UPDATE `medLog` SET collected = '1' WHERE `medLog`.`id` = $id";
    
        if(mysqli_query($conexion, $sql))
            echo "OK"; 
           
        }
    
     }


?>
