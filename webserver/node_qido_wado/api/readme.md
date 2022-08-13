# Conquest Dicom API using QIDO and WADO-URI to Conquest PACS Server

This project is a proof of concept to create an API with node.js that is compliant to QIDO and WADO-URI for accessing [Conquest PACS server]{https://ingenium.home.xs4all.nl/dicom.html}, an open-source DICOM server. 

DICOMweb is a term applied to the family of RESTful DICOM services defined for sending, retrieving and querying for medical images and related information.

The intent is to provide a light-weight mobile device and web browser friendly mechanism for accessing images, which can be implemented by developers who have minimal familiarity with the DICOM standard and which uses consumer application friendly mechanisms like http, JSON and media types (like "image/jpeg") to the maximum extent possible.

The standard is formally defined in DICOM PS3.18 Web Services.

The DICOMweb services are distinguished from other DICOM web services by the suffix "-RS", indicating their RESTful nature.

The family consists primarily of:
* WADO-RS for retrieval of DICOM PS3.10 files, meta data in XML or JSON forms, bulk data separated from the meta data and rendered consumer format images
* STOW-RS for storage (sending) of DICOM PS3.10 files or separated meta data and bulk data
* QIDO-RS for querying collections (databases, registries) of DICOM objects

A key feature of the WADO-RS services is the ability to retrieve entire studies and series rather than needing repeated request for individual instances.

The DICOM standard specifies the file format and the network protocols for medical imaging. The DICOM standard defines, in its part PS 3.18, a Web-based service for accessing images that is known as WADO (Web Access to DICOM Persistent Objects). WADO notably allows healthcare professionals to preview and download medical images using standard Web browsers.


The code attached to this project shows how to implement a basic QIDO/WADO-URI API acessing [Conquest PACS server]{https://ingenium.home.xs4all.nl/dicom.html}, an open-source DICOM server. 
To WADO, it can return either raw DICOM images, or a JPEG, BMP or GIF preview of these images. 
To QIDO, it can return json response to query studies, series and instances. 

Not all feautures from QIDO API have been implemented.

The API works by running lua scripts remotely in the target conquest server to generate the requested data. The scripts and data processing are provided by lua script ran in the api side using a dgate executable. The configure the AE, IP address and PORT of the target server go to dicomRoutes.js file and edit:
```sh
let CQPORT = process.env.CQPORT || "5678";
let CQIP = process.env.CQIP || "127.0.0.1";
let CQAE = process.env.CQAE || "CONQUESTSRV1";
let APIFOLDER = `${global.appRoot}/api/dgate`;
```
of course, you can use enviroment variables as well. To use it edit the .env file in the root folder:
Sample of .env  file:
```sh
CQPORT=5678
CQIP=127.0.0.1
CQAE=CONQUESTSRV1
```
The default AE is CONQUESTSRV1, IP: 127.0.0.1, PORT: 5678. The conquest server does not need to run on the same computer as the API web server.

## Requirements
* Install [Conquest PACS server]{https://ingenium.home.xs4all.nl/dicom.html}, an open-source DICOM server and node.js
* You will need at least the `version 1.5.0c test Release`  from https://github.com/marcelvanherk/Conquest-DICOM-Server with updates after September 7, 2022
* Install [Node.js]{https://nodejs.org/en/}

## Installation
You need to write the following commands on the terminal screen so that you can run the project locally.
```sh
1. unpack this package in a folder
2. cd your-folder-unpacked
3. npm install
4. npm start
```

The application is running on [localhost](http://localhost:3002)
//http:localhost:3002
The port number is set editing the file app.js in the root folder:
```sh
const express = require("express");
const app = express();
const port = 3002; ---> change here
```

## Notes: 
The following Search Query Parameters: match, fuzzymatching, includefield='all' are not implemented
This application is a proof of concept, not a complete implementation from QIDO and WADO RESTfull services.

## Routes 

** Query Studies **
- `/rs/studies{?search*}`
for example:
 rs/studies?PatientID=11235813
 rs/studies?PatientID=11235813&StudyDate=20130509
Data returned is according to the DICOM standard
 
** Query Series in a Study **
- `/rs/studies/{study}/series{?search*}`
for example:
rs/studies/1.3.46.670589.30.1.6.1.116520970982.1491317778640.1/series/1.3.46.670589.30.1.6.1.116520970982.1491318133781.1
Data returned is according to the DICOM standard

** Query Instances in a Series **
- `/studies/{study}/series/{series}/instances{?search*}`
for sample:
rs/studies/1.3.46.670589.30.1.6.1.116520970982.1491317778640.1/series/1.3.46.670589.30.1.6.1.116520970982.1491318133781.1/instances
Data returned is according to the DICOM standard
 
** WADO **
- `/wadouri`
Sample:
rs/wadouri?requestType=WADO&studyUID=2.16.840.1.113669.632.20.121711.10000161048&seriesUID=1.3.46.670589.11.17521.5.0.3124.2008081915302126599&objectUID=1.3.46.670589.11.17521.5.0.3124.2008081915354146613&contentType=application/dicom

rs/wadouri?requestType=WADO&studyUID=1.3.46.670589.30.1.6.1.116520970982.1491317778640.1&seriesUID=1.3.46.670589.30.1.6.1.116520970982.1491318137421.1&objectUID=1.3.46.670589.30.1.6.1.116520970982.1491318134156.2&contentType=image/jpeg&rows=128&columns=128

Implemented WADO parameters are:
studyUID
seriesUID
objectUID
requestType; must be "WADO"
contentType; can be "application/dicom", "image/jpeg", "image/gif", "image/bmp", "text/plain" or "text/html"
rows; partially implemented to downsize dicom images
columns; partially implemented to downsize dicom images
region; to crop a dicom image region prior to conversion e.g. to jpeg
anomymize; if set to "yes" will provide anonymised dicom image, no anonumisation if not passed or passed as "".
  If set to another value will generate data for the value as anonymisation "stage"
windowCenter; center of grayscale range for windowing
windowWidth; size of grayscale range for windowing
frameNumber; frame of a multiframe object - if not passed image/gif will be animated at 5 fps
imageQuality; compression level for jpeg images
bridge; AE of server to be WADO bridge for
annotation; is NOT supported
presentationUID; to recompress dicom images with supported values
  1.2.840.10008.1.2.4.50
  1.2.840.10008.1.2.4.51
  1.2.840.10008.1.2.4.53
  1.2.840.10008.1.2.4.55
  1.2.840.10008.1.2.4.57
  1.2.840.10008.1.2.4.70
  1.2.840.10008.1.2.4.80
  1.2.840.10008.1.2.4.81
  1.2.840.10008.1.2.4.90
  1.2.840.10008.1.2.4.91

** Custom api **
* Get a single DICOM image
- '/dicomSlice/{PatientID:SOPInstanceUID}'
Sample
rs/dicomSlice/12345:1.3.46.670589.30.1.6.1.116520970982.1491318134156.2

* Convert a DICOM image to jpg/bmp or gif
- '/dicomConverter/{Graphic}/{Size}/{PatientID:SOPInstanceUID}'
Sample:
rs/dicomConverter/12345:1.3.46.670589.30.1.6.1.116520970982.1491318134156.2/jpg/128/12345:1.3.46.670589.30.1.6.1.116520970982.1491318134156.2

* Query dicom studies
- '/dicomStudies'
Sample:
post rs/dicomStudies
request {"PatientID":"ABC","StudyDate":""}
response contains TransferSyntaxUID, StudyDate, StudyTime, QueryRetrieveLevel, PatientName, PatientID, PatientBirthDate, StudyInstanceUID for each matching study, wildcards can be used

* Query dicom series
- '/dicomSeries'
Sample:
post rs/dicomSeries
request {"PatientID":"ABC","StudyInstanceUID":"1.2.826.0.1.3680043.2.135.737059.58899160.7.1563455506.484.35"}
response contains TransferSyntaxUID, SeriesDate, SeriesTime, QueryRetrieveLevel, Modality, SeriesDescription, PatientID, StudyInstanceUID, SeriesInstanceUID for each matching series

* Query dicom images
- '/dicomImages'
Sample:
post rs/dicomImages
request {"PatientID":"ABC","SeriesInstanceUID":"1.2.826.0.1.3680043.2.135.737059.58899160.7.1563455506.484.36"}
response contains TransferSyntaxUID, SOPInstanceUID, ContentDate, QueryRetrieveLevel, PatientName, PatientID, StudyInstanceUID, SeriesInstanceUID, InstanceNumber, NumberOfFrames for each matching image
