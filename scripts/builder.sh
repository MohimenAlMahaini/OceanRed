# This Script build The Anngular Application and moves it to ESP32 /data/www/ folder
# Author: Mohimen Al Mahaini

#!/bin/bash
clear

checkMark="\xE2\x9C\x94" 
crossMark="\xE2\x9D\x8C" 

# build the Angular Application 
buildAngular()
{
    # move to the working directory
    cd ../angular-on-esp/esp32-configuration-webpage

    ng build --configuration=production
    if [ $? -eq 0 ]; then
        echo -e "$checkMark Angular build succeeded!"
    else
        echo -e "$crossMark Angular built has failed!"
        exit 1  
    fi
}

# copy files to data/www, remove the hash from file name, compress the files with gzip, and edit index.html
prepareFiles()
{
    cd dist/esp32-configuration-webpage/ 
    
    cp * ../../../../ESP-OceanRed-PIO/data/www 

    cd ../../../../ESP-OceanRed-PIO/data/www 
    for file in *
    do
        echo "$file"
        if [[ $file == main.*  ||  $file == runtime.*  ||  $file == styles.*  ||  $file == polyfills.* ]]
        then
            fileName=$(echo "$file" | cut -d '.' -f1,3-) # on Ubuntu cut -d '.' -f1,3- On Mac cut -d '.' -f1 -f3 
            echo $fileName
            mv $file $fileName
            gzip $fileName
            # sed -i '' "s/$file/$filename/g" index.html
        fi
        # echo "$file"
    done    
}

buildAngular
prepareFiles

if [ $? -eq 0 ]
then
    echo -e "$checkMark Files have been sorted."
else
    echo -e "$crossMark Something went wrong with sorting files!"
    exit 1
fi


echo '<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="utf-8">
  <title>Esp32ConfigurationWebpage</title>
  <base href="/">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" type="image/x-icon" href="favicon.ico">
  <link rel="stylesheet" href="styles.css">
</head>

<body>
  <app-root></app-root>
  <script src="runtime.js" defer></script>
  <script src="polyfills.js" defer></script>
  <script src="main.js" defer></script>
</body>
</html>' > index.html

