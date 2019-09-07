<?php
include('config.php');

$ip = file_get_contents(Config::HOST_FILE);
$src = 'http://' . $ip . '/';

?><!DOCTYPE HTML5>
<html>
<head>
	<title>Aquarium light</title>
	<style type="text/css">
		body, html {
			margin: 0; padding: 0; height: 100%; overflow: hidden;
		}
		#content {
			position:absolute; left: 0; right: 0; bottom: 0; top: 0px; 
		}
	</style>
</head>
<body>
	<div id="content">
		<iframe width="100%" height="100%" frameborder="0" src="<?php echo $src; ?>" />
	</div>
</body>