enable php stuff on linux:

   32  sudo apt install php libapache2-mod-php
   44  a2enmod php7.4
   54  a2enmod rewrite
   60  nano /etc/apache2/apache2.conf 
?  39  nano /etc/apache2/mods-enabled/dir.conf
   62  systemctl restart apache2

   36  cp -r /mnt/dicom/api /var/www/html/
   37  cp -r /mnt/dicom/app /var/www/html/
   48  chown -R www-data /var/www/html/api
   49  chown -R www-data /var/www/html/app
   51  chmod 777 /var/www/html/api/dicom/servertask
   
   64  tail /var/log/apache2/error.log 
   
   71  /var/www/html/api/dicom/servertask "print(1)"
   72  /var/www/html/api/dicom/servertask "--echo:CONQUESTSRV1"
   77  apt install wget
   80  wget https://unpkg.com/@ohif/viewer@1.0.3/dist/index.umd.js -O ohif1.03_min.js
   83  cp ohif1.03_min.js /var/www/html/app/ohif/

   66  nano /var/www/html/api/dicom/qido.php 
   84  nano /var/www/html/api/dicom/index.php 
   88  nano /var/www/html/api/dicom/wado.php 
   89  nano /var/www/html/app/ohif/index.html 
   91  cp -R /var/www/html/app/ohif/* /var/www/html/
   92  chown www-data /var/www/html/ohif1.03_min.js 
   94  history >history.txt
