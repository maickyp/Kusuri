<?php
    date_default_timezone_set('America/Mexico_City');

    //$postdata = file_get_contents("php://input");  
    //$obj = json_decode($postdata);
    $password = $_POST['key'];
    $med = $_POST['med'];
    $tube = $_POST['tube'];
    //$password = $obj->{'key'};
    //echo "{OK}";
    if($password == 'PASSWORD'){
        $fecha = new DateTime();
        $timestamp = $fecha->format('Y-m-d H:i:s');
        $host= "host";
        $username= "username";
        $dbname= "dbname";
        $med = $_POST['med'];
        $tube = $_POST['tube'];
        
        $connect=mysqli_connect($host,$username,$password,$dbname);
        
        $sql = "INSERT INTO `medLog`(`id`, `fecha`, `tube`, `medicine`, `collected`) VALUES (NULL, '$timestamp', $tube, '$med', 0)";
        
        if(mysqli_query($connect, $sql))
            echo "OK";
       
        
        }      
    
    ?>
