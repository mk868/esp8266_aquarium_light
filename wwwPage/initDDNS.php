<?php
include('config.php');

if(Config::TOKEN == $_GET['token']){
	file_put_contents(Config::HOST_FILE, $_GET['ip']);
	echo 1;
} else {
	echo 0;
}

?>