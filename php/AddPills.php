<?php

$host= "host";
$username= "username";
$password= "password";
$dbname= "dbname";

$connect=mysqli_connect($host,$username,$password,$dbname);

$tubeNum=$_POST["tubeNum"];
$level=$_POST["level"];

mysqli_query($connect,"UPDATE medNames SET level='$level' WHERE tubeNum='$tubeNum'");

echo "OK"

?>