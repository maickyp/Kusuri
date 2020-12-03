<?php
    //$postdata = file_get_contents("php://input");  
    //$obj = json_decode($postdata);
    $password = $_POST['key'];
    //$password = $obj->{'key'};
    //echo "{OK}";
if($password == "PASSWORD"){
        $host= "host";
        $user= "username";
        $dbname= "dbname";
        $reg = $_POST['reg'];
            $conexion = mysqli_connect($host, $user, $password, $dbname);

        
        if($reg==1){
            $level = $_POST['level1'];
            $sql= "UPDATE `medNames` SET level = $level WHERE `medNames`.`tubeNum` = 1";

        if(mysqli_query($conexion, $sql))
            echo "OK";
        }
        
        if($reg==2){
            $level = $_POST['level2'];
            $sql= "UPDATE `medNames` SET level = $level WHERE `medNames`.`tubeNum` = 2";

        if(mysqli_query($conexion, $sql))
            echo "OK";
        }
        
        if($reg==3){
            $level = $_POST['level1'];
            $sql= "UPDATE `medNames` SET level = $level WHERE `medNames`.`tubeNum` = 1";
            
            mysqli_query($conexion, $sql) or die;
            
            $level = $_POST['level2'];
            $sql= "UPDATE `medNames` SET level = $level WHERE `medNames`.`tubeNum` = 2";

        if(mysqli_query($conexion, $sql))
            echo "OK";
        }
        
        }      
    
    ?>