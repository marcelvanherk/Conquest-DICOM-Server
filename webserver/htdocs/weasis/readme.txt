Weasis requires install of the java runtime

put the contents of the app folder in
https://sourceforge.net/projects/dcm4che/files/latest/download

here.

weasis can be selected as choice in the web installer

weasis://$dicom:get -w "https://nroduit.github.io/samples/demo/compression3.xml"

<?xml version="1.0" encoding="UTF-8" ?>
<manifest uid="706f1a06-6581-4284-880e-bcb457422dc8" xmlns="http://www.weasis.org/xsd/2.5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
	<arcQuery arcId="local" baseUrl="https://nroduit.github.io/samples/demo/">
		<Patient PatientBirthDate="19500704" PatientID="MF-compression-frag" PatientName="TEST^Compression" PatientSex="M">
			<Study AccessionNumber="9995000" StudyDate="20061219" StudyDescription="Test compression, multiframe and fragments" StudyID="05000" StudyInstanceUID="1.3.6.1.4.1.5962.1.2.5000.1166546115.14677" StudyTime="173500">
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="Original: Sagittal,1 stack,1 dimension,temporal position,ext loc ref" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677" SeriesNumber="1">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="RLE Compression" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.1" SeriesNumber="2">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.1" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.1"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG Lossless Compression" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.8" SeriesNumber="3">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.8" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.8"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG Compression and 1024 fragments with no offset table" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.10" SeriesNumber="4">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.10" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.10"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG Compression and 1024 fragments" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.9" SeriesNumber="5">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.9" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.9"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG-LS Lossless Compression" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.7" SeriesNumber="6">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.7" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.7"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG-LS Compression and 1024 fragments with no offset table" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.4" SeriesNumber="7">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.4" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.4"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG-LS Compression and 1024 fragments" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.3" SeriesNumber="8">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.3" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.3"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG-LS Lossy Compression" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.6" SeriesNumber="9">
					<Instance DirectDownloadFile="1.2.826.0.1.3680043.2.1143.2615233261306178997410318463898374983.6" InstanceNumber="1" SOPInstanceUID="1.2.826.0.1.3680043.2.1143.2615233261306178997410318463898374983.6"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG200 Lossless Compression" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.12" SeriesNumber="10">
					<Instance DirectDownloadFile="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.12" InstanceNumber="1" SOPInstanceUID="1.3.6.1.4.1.5962.1.1.5000.3.1.1166546115.14677.12"/>
				</Series>
				<Series DirectDownloadThumbnail="thumb/tumb_12960046888933873192.jpg" Modality="MR" SeriesDescription="JPEG200 Lossy Compression" SeriesInstanceUID="1.3.6.1.4.1.5962.1.3.5000.3.1166546115.14677.11" SeriesNumber="11">
					<Instance DirectDownloadFile="1.2.826.0.1.3680043.2.1143.7567016219912961108887122553257011601.11" InstanceNumber="1" SOPInstanceUID="1.2.826.0.1.3680043.2.1143.7567016219912961108887122553257011601.11"/>
				</Series>
			</Study>
		</Patient>
	</arcQuery>
</manifest>

<?xml version="1.0" encoding="UTF-8" ?>
<manifest xmlns="http://www.weasis.org/xsd/2.5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
    <arcQuery additionnalParameters="" arcId="1001" baseUrl="http://archive-weasis.rhcloud.com/archive/wado" requireOnlySOPInstanceUID="false">
        <Patient PatientID="H13885_9M" PatientName="TEST NON SQUARE PIXELS" PatientSex="F">
            <Study AccessionNumber="" ReferringPhysicianName="" StudyDate="20130711" StudyDescription="TEST NON SQUARE PIXELS" StudyID="PKD" StudyInstanceUID="2.16.756.5.5.100.397184556.14391.1373576413.1508" StudyTime="170013">
                <Series Modality="US" SeriesDescription="NON SQUARE PIXELS: PIXEL ASPECT RATIO" SeriesInstanceUID="1.2.40.0.13.1.1.87878503032592846377547034671833520632" SeriesNumber="2">
                    <Instance InstanceNumber="107" SOPInstanceUID="1.2.40.0.13.1.1.126082073005720329436273995268222863740"/>
                </Series>
                <Series Modality="MR" SeriesDescription="NON SQUARE PIXELS: PIXEL SPACING" SeriesInstanceUID="2.16.756.5.5.100.397184556.7220.1373578035.1" SeriesNumber="40001">
                    <Instance InstanceNumber="1" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578035.1.0"/>
                    <Instance InstanceNumber="2" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578035.1.1"/>
                    <Instance InstanceNumber="3" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578035.1.2"/>
                    <Instance InstanceNumber="4" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578035.1.3"/>
                </Series>
                <Series Modality="MR" SeriesDescription="NON SQUARE PIXELS: PIXEL SPACING" SeriesInstanceUID="2.16.756.5.5.100.397184556.7220.1373578664.2" SeriesNumber="50001">
                    <Instance InstanceNumber="1" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578664.2.0"/>
                    <Instance InstanceNumber="2" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578664.2.1"/>
                    <Instance InstanceNumber="3" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578664.2.2"/>
                    <Instance InstanceNumber="4" SOPInstanceUID="2.16.756.5.5.100.397184556.7220.1373578664.2.3"/>
                </Series>
            </Study>
        </Patient>
    </arcQuery>
</manifest>