<?php
    $password = $_POST['key'];
    if($password == "PASSWORD"){
        $host= "host";
        $user= "username";
        $dbname= "dbname";
        
        $conexion = mysqli_connect($host, $user, $password, $dbname);
        
        $sql = "SELECT * FROM `medNames` ORDER BY id";
        $result = mysqli_query($conexion, $sql);
        //$row = mysqli_fetch_assoc($result);
        $numrows= mysqli_num_rows($result);
        
        $response = "{";
        
        if ($numrows > 0) {
                // output data of each row
                $contador = 1;
                    while($row = mysqli_fetch_assoc($result)) {
                        $response = $response . "\"nameMed".$contador."\":\"".$row["nameMed"]."\",\"tubeNum".$contador."\":\"".$row["tubeNum"]."\",\"doseTime".$contador."\":\"".$row["doseTime"]."\",\"nextDose".$contador."\":\"".$row["nextDose"]."\",\"level".$contador."\":\"".$row["level"]."\"";
                        if($contador < $numrows)
                            $response = $response . ",";
                        $contador = $contador+1;
                        }
        }
        
        $response = $response."}";
        echo $response;
        }
        
        ?>
