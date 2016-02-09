<?php


#
# The following should be the domain of this web server - where this script is
#   deployed.
#
$site = "www.ral.ucar.edu";

#
# The following should be the port serving content for this site (typically 80)
#
$port = 80;

#
# The following should be the path of the proxy on the production server
#   relative to the production web server's $ServerRoot
#
$uri = "/projects/rdwx_mdss/proxy.php";

$timeout = 10;

if ($fp = fsockopen($site, $port, $errno, $errstr, $timeout) ) {
    #
    # Server is up, read the result.
    #
    $query_string = $_SERVER['QUERY_STRING'];
    fputs($fp, "GET $uri?$query_string HTTP/1.0\r\nHost: $site\r\n\r\n");

    #
    # Skip the HTML header of the reply.
    #
    while(!feof($fp) ) {
        # Possibly buffer overflow problems here with a fixed buffer size...
        $line = fgets($fp, 4096);

        #
        # Check for Content-type and save it
        # (stripping off trailing whitespace)
        #
        if (strncasecmp($line,"Content-type",12)==0)
        {
            $contentType = rtrim($line);
        }

        #
        # Testing for white space catches "^\r\n$", for example.
        #
        if (preg_match("/^\s*$/", $line) ) {
            break;
        }
    }

    # Read the body.  Binary safe.
    $buffer;
    while(!feof($fp) ) {
        $buffer .= fread($fp, 1024);
    }
    fclose($fp);

    Header("$contentType");
    print($buffer);

}
else {
    echo "$errstr ($errno)<br>\n";
}

?>

