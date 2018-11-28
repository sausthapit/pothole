// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

/**
* Tutorial solution for Walkthrough 2 in the XMP Programmers Guide, Modifying XMP
* Demonstrates how to open a file for update, and modifying the contained XMP before writing it back to the file.
*/

#include "AddMetadata.h"



using namespace std;
using namespace AdobeXMPCore;
const char* NS_DC_TERMS = "http://purl.org/dc/terms/";
void GetLocalizedText(spIArrayNode titleNode, const char* specificLang, const char* genericLang, string lang)
{
	AdobeXMPCore::spINode currItem;
	const size_t itemLim = titleNode->ChildCount();
	size_t itemNum;

	spISimpleNode xmlLangQualifierNode, currItemNode;
	for (itemNum = 1; itemNum <= itemLim; ++itemNum)
	{
		currItem = titleNode->GetNodeAtIndex(itemNum);
		if (currItem != NULL)
		{
			xmlLangQualifierNode = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
			if (!strcmp(xmlLangQualifierNode->GetValue()->c_str(), specificLang)) {
				currItemNode = currItem->ConvertToSimpleNode();
				cout << "dc:title in" << " " << lang << " " << currItemNode->GetValue()->c_str() << endl;
				return;
			}
		}
	}

	if (*genericLang != 0)
	{
		// Look for the first partial match with the generic language.
		const size_t genericLen = strlen(genericLang);
		for (itemNum = 1; itemNum <= itemLim; ++itemNum) {
			currItem = titleNode->GetNodeAtIndex(itemNum);
			xmlLangQualifierNode = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
			XMP_StringPtr currLang = xmlLangQualifierNode->GetValue()->c_str();
			const size_t currLangSize = xmlLangQualifierNode->GetValue()->size();
			if ((currLangSize >= genericLen) &&
				!strncmp(currLang, genericLang, genericLen) &&
				((currLangSize == genericLen) || (currLang[genericLen] == '-')))
			{
				currItemNode = currItem->ConvertToSimpleNode();
				cout << "dc:title in" << " " << lang << " " << currItemNode->GetValue()->c_str() << endl;
				return;
			}
		}
	}

	// Look for an 'x-default' item.
	for (itemNum = 1; itemNum <= itemLim; ++itemNum) {
		currItem = titleNode->GetNodeAtIndex(itemNum);
		xmlLangQualifierNode = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
		if (!strcmp(xmlLangQualifierNode->GetValue()->c_str(), "x-default")) {
			currItemNode = currItem->ConvertToSimpleNode();
			cout << "dc:title in" << " " << lang << " " << currItemNode->GetValue()->c_str() << endl;
			return;
		}
	}

	// Everything failed, choose the first item.
	currItem = titleNode->GetNodeAtIndex(1);
	currItemNode = currItem->ConvertToSimpleNode();
	cout << "dc:title in" << " " << lang << " " << currItemNode->GetValue() << endl;
	return;

}



/**
* Display some property values to the console
*
* meta - a pointer to the XMP object that will have the properties read
*/
void displayPropertyValues(AdobeXMPCore::spIMetadata metaNode)
{
	// Read a simple property
	AdobeXMPCore::spISimpleNode simpleNode = metaNode->GetSimpleNode(kXMP_NS_XMP, AdobeXMPCommon::npos, "CreatorTool", AdobeXMPCommon::npos);
	if (simpleNode != NULL)
	{
		string simpleNodeValue = simpleNode->GetValue()->c_str();
		cout << "CreatorTool = " << simpleNodeValue << endl;
	}
	AdobeXMPCore::spISimpleNode extentNode = metaNode->GetSimpleNode(NS_DC_TERMS, AdobeXMPCommon::npos, "extent", AdobeXMPCommon::npos);
	if (extentNode != NULL)
	{
		string simpleNodeValue = extentNode->GetValue()->c_str();
		cout << "Extent = " << simpleNodeValue << endl;
	}
	AdobeXMPCore::spISimpleNode descNode = metaNode->GetSimpleNode(kXMP_NS_DC, AdobeXMPCommon::npos, "description", AdobeXMPCommon::npos);
	if (descNode != NULL)
	{
		string simpleNodeValue = descNode->GetValue()->c_str();
		cout << "Description = " << simpleNodeValue << endl;
	}
	// Get the first element in the dc:creator array
	AdobeXMPCore::spIArrayNode arrayNode = metaNode->GetArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "creator", AdobeXMPCommon::npos);
	if (arrayNode != NULL)
	{
		AdobeXMPCore::spISimpleNode arrayNodeChild = arrayNode->GetSimpleNodeAtIndex(1);
		if (arrayNodeChild != NULL)
		{
			string arrayNodeChildValue = arrayNodeChild->GetValue()->c_str();
			cout << "dc:creator[1] = " << arrayNodeChildValue << endl;
		}
		
		AdobeXMPCore::spISimpleNode arrayNodeChild1 = arrayNode->GetSimpleNodeAtIndex(2);
		if (arrayNodeChild1 != NULL)
		{
			string arrayNodeChildValue1 = arrayNodeChild1->GetValue()->c_str();
			cout << "dc:creator[2] = " << arrayNodeChildValue1 << endl;
		}
	}

	// Get the the entire dc:subject array 
	AdobeXMPCore::spIArrayNode subjectArray = metaNode->GetArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "subject", AdobeXMPCommon::npos);
	if (subjectArray != NULL)
	{
		int arraySize = subjectArray->ChildCount();
		for (int i = 1; i <= arraySize; i++)
		{
			AdobeXMPCore::spISimpleNode subjectChild = subjectArray->GetSimpleNodeAtIndex(i);
			if (subjectChild != NULL)
			{
				string propValue = subjectChild->GetValue()->c_str();
				cout << "dc:subject[" << i << "] = " << propValue << endl;
			}
		}
	}
	// Get the dc:title for English and French
	AdobeXMPCore::spIArrayNode titleNode = metaNode->GetArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "title", AdobeXMPCommon::npos);
	if (titleNode != NULL)
	{
		GetLocalizedText(titleNode, "en-US", "en", "English");
		GetLocalizedText(titleNode, "fr-FR", "fr", "French");
	}

	// Get dc:MetadataDate
	AdobeXMPCore::spISimpleNode dateNode = metaNode->GetSimpleNode(kXMP_NS_XMP, AdobeXMPCommon::npos, "MetadataDate", AdobeXMPCommon::npos);
	if (dateNode != NULL)
	{
		string date = dateNode->GetValue()->c_str();
		cout << "meta:MetadataDate = " << date << endl;
	}

	// See if the flash struct exists and see if it was used
	AdobeXMPCore::spIStructureNode flashNode = metaNode->GetStructureNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "Flash", AdobeXMPCommon::npos);
	if (flashNode != NULL)
	{
		AdobeXMPCore::spISimpleNode field = flashNode->GetSimpleNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "Fired", AdobeXMPCommon::npos);
		if (field != NULL)
		{
			string fieldValue = field->GetValue()->c_str();
			cout << "Flash Used = " << fieldValue << endl;
		}
	}
	AdobeXMPCore::spISimpleNode gpsLatitude = metaNode->GetSimpleNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "GPSLatitude", AdobeXMPCommon::npos);
	if (gpsLatitude != NULL)
	{
		string lat = gpsLatitude->GetValue()->c_str();
		cout << "exif:GPSLatitude: " << lat << endl;
	}
	AdobeXMPCore::spISimpleNode gpsLongitude = metaNode->GetSimpleNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "GPSLongitude", AdobeXMPCommon::npos);
	if (gpsLongitude != NULL)
	{
		string lon = gpsLongitude->GetValue()->c_str();
		cout << "exif:GPSLongitude: " << lon << endl;
	}
	AdobeXMPCore::spISimpleNode gpsAltitude = metaNode->GetSimpleNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "GPSAltitude", AdobeXMPCommon::npos);
	if (gpsAltitude != NULL)
	{
		string alt = gpsAltitude->GetValue()->c_str();
		cout << "exif:GPSAltitude: " << alt << endl;
	}
	AdobeXMPCore::spISimpleNode gpsTimeStamp = metaNode->GetSimpleNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "GPSTimeStamp", AdobeXMPCommon::npos);
	if (gpsTimeStamp != NULL)
	{
		string tstamp = gpsTimeStamp->GetValue()->c_str();
		cout << "exif:GPSTimeStamp: " << tstamp << endl;
	}

	cout << "----------------------------------------" << endl;
}

/**
* Creates an XMP object from an RDF string.  The string is used to
* to simulate creating and XMP object from multiple input buffers.
* The last call to ParseFromBuffer has no kXMP_ParseMoreBuffers options,
* thereby indicating this is the last input buffer.
*/
SXMPMeta createXMPFromRDF()
{
	const char * rdf =
		"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
		"<rdf:Description rdf:about='' xmlns:dc='http://purl.org/dc/elements/1.1/'>"
		"<dc:subject>"
		"<rdf:Bag>"
		"<rdf:li>XMP</rdf:li>"
		"<rdf:li>SDK</rdf:li>"
		"<rdf:li>Sample</rdf:li>"
		"</rdf:Bag>"
		"</dc:subject>"
		"<dc:format>image/tiff</dc:format>"
		"</rdf:Description>"
		"</rdf:RDF>";

	SXMPMeta meta;
	// Loop over the rdf string and create the XMP object
	// 10 characters at a time 
	int i;
	for (i = 0; i < (long)strlen(rdf) - 10; i += 10)
	{
		meta.ParseFromBuffer(&rdf[i], 10, kXMP_ParseMoreBuffers);
	}

	// The last call has no kXMP_ParseMoreBuffers options, signifying 
	// this is the last input buffer
	meta.ParseFromBuffer(&rdf[i], (XMP_StringLen)strlen(rdf) - i);
	return meta;

}


/**
* Writes an XMP packet in XML format to a text file
*
* rdf - a pointer to the serialized XMP
* filename - the name of the file to write to
*/
void writeRDFToFile(string * rdf, string filename)
{
	ofstream outFile;
	outFile.open(filename.c_str(), ios::out);
	outFile << *rdf;
	outFile.close();
}

/**
* Initializes the toolkit and attempts to open a file for updating its metadata.Initially
* an attempt to open the file is done with a handler, if this fails then the file is opened with
* packet scanning.Once the file is open several properties are read and displayed in the console.
*
* Several properties are then modified, first by checking for their existence and then, if they
* exist, by updating their values.The updated properties are then displayed again in the console.
*
* Next a new XMP object is created from an RDF stream, the properties from the new XMP object are
* appended to the original XMP object and the updated properties are displayed in the console for
* last time.
*
* The updated XMP object is then serialized in different formats and written to text files.Lastly,
*the modified XMP is written back to the resource file.
*/
//

bool setSimpleProperty(AdobeXMPCore::spIMetadata metaNode, const char * nameSpace, const char * tag, const char * value)
{
	//Set the simple property value, will create the property in the given nameSpace if not already present
	try {
		AdobeXMPCore::spISimpleNode simpleNode = metaNode->GetSimpleNode(nameSpace, AdobeXMPCommon::npos, tag, AdobeXMPCommon::npos);
		if (simpleNode == NULL)
		{
			simpleNode = AdobeXMPCore::ISimpleNode::CreateSimpleNode(nameSpace, AdobeXMPCommon::npos, tag, AdobeXMPCommon::npos);
			metaNode->InsertNode(simpleNode);
		}
		simpleNode->SetValue(value, AdobeXMPCommon::npos);
		
	}
	catch (std::runtime_error err) {
		std::cout << "Could not set the property" << std::endl;
	}
	return true;
}

int addXMP(string filename ,std::vector<std::string> gpsString)
//int main(int argc, const char * argv[])
{

	//string filename = string("img.jpg");

	if (!SXMPMeta::Initialize())
	{
		cout << "Could not initialize toolkit!";
		return -1;
	}

	XMP_OptionBits options = 0;
#if UNIX_ENV
	options |= kXMPFiles_ServerMode;
#endif

	// Must initialize SXMPFiles before we use it
	if (SXMPFiles::Initialize(options))
	{
		try
		{
			// Options to open the file with - open for editing and use a smart handler
			XMP_OptionBits opts = kXMPFiles_OpenForUpdate | kXMPFiles_OpenUseSmartHandler;

			bool ok;
			SXMPFiles myFile;
			std::string status = "";

			// First we try and open the file
			ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
			if (!ok)
			{
				status += "No smart handler available for " + filename + "\n";
				status += "Trying packet scanning.\n";

				// Now try using packet scanning
				opts = kXMPFiles_OpenForUpdate | kXMPFiles_OpenUsePacketScanning;
				ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
			}

			// If the file is open then read get the XMP data
			if (ok)
			{
				cout << status << endl;
				cout << filename << " is opened successfully" << endl;

				
				string actualPrefix;
				SXMPMeta::RegisterNamespace(NS_DC_TERMS, "dcterms", &actualPrefix);
				// Create the xmp object and get the xmp data
				SXMPMeta meta;
				myFile.GetXMP(&meta);
				string buffer;
				meta.SerializeToBuffer(&buffer);
				AdobeXMPCore::spIDOMImplementationRegistry DOMRegistry = AdobeXMPCore::IDOMImplementationRegistry::GetDOMImplementationRegistry();
				AdobeXMPCore::spIDOMParser parser = DOMRegistry->GetParser("rdf");
				AdobeXMPCore::spIMetadata metaNode = parser->Parse(buffer.c_str(), buffer.size());

				
				
				// Display some properties in the console
				displayPropertyValues(metaNode);

				///////////////////////////////////////////////////
				// Now modify the XMP
				AdobeXMPCore::spISimpleNode simpleNode = metaNode->GetSimpleNode(kXMP_NS_XMP, AdobeXMPCommon::npos, "CreatorTool", AdobeXMPCommon::npos);
				
				if (simpleNode!=NULL)
				simpleNode->SetValue("Updated By Cranfied University", AdobeXMPCommon::npos);

				// Update the MetadataDate
				XMP_DateTime dt;
				SXMPUtils::CurrentDateTime(&dt);
				string  date;
				SXMPUtils::ConvertFromDate(dt, &date);
				AdobeXMPCore::spISimpleNode dateNode = metaNode->GetSimpleNode(kXMP_NS_XMP, AdobeXMPCommon::npos, "MetadataDate", AdobeXMPCommon::npos);
				if (dateNode == NULL)
				{
					dateNode = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_XMP, AdobeXMPCommon::npos, "MetadataDate", AdobeXMPCommon::npos);

				}
				dateNode->SetValue(date.c_str(), AdobeXMPCommon::npos);

				// Add an item onto the dc:creator array
				AdobeXMPCore::spIArrayNode arrayNode = metaNode->GetArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "creator", AdobeXMPCommon::npos);
				
				// If the array does not exist, it will be created
				if (arrayNode == NULL)
				{
					arrayNode = AdobeXMPCore::IArrayNode::CreateUnorderedArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "creator", AdobeXMPCommon::npos);
				}
					AdobeXMPCore::spINode creatorChild1 = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_DC, AdobeXMPCommon::npos, "AuthorName", AdobeXMPCommon::npos, "Saurav", AdobeXMPCommon::npos);
					
					AdobeXMPCore::spINode creatorChild2 = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_DC, AdobeXMPCommon::npos, "AnotherAuthorName", AdobeXMPCommon::npos, "EWIC", AdobeXMPCommon::npos);
					arrayNode->AppendNode(creatorChild1);
					arrayNode->AppendNode(creatorChild2);
				//metaNode->InsertNode(arrayNode);

				// Add gps info
				
				setSimpleProperty(metaNode, kXMP_NS_EXIF, "GPSTimeStamp", gpsString.at(0).c_str());
				setSimpleProperty(metaNode, kXMP_NS_EXIF, "GPSLatitude", gpsString.at(1).c_str());
				setSimpleProperty(metaNode, kXMP_NS_EXIF, "GPSLongitude", gpsString.at(2).c_str());
				setSimpleProperty(metaNode, kXMP_NS_EXIF, "GPSAltitude", gpsString.at(3).c_str());

				setSimpleProperty(metaNode, kXMP_NS_DC, "description", "Image of pothole.");
				setSimpleProperty(metaNode, kXMP_NS_DC, "type", "Image");
				setSimpleProperty(metaNode, kXMP_NS_DC, "subject", "Image");
				setSimpleProperty(metaNode, NS_DC_TERMS, "extent", gpsString.at(4).c_str());
				// Display the properties again to show changes
				cout << "After update:" << endl;
				displayPropertyValues(metaNode);

				AdobeXMPCore::spIDOMSerializer serializer = DOMRegistry->GetSerializer("rdf");
				AdobeXMPCore:: spcINameSpacePrefixMap defaultMap = AdobeXMPCore::INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
				std::string serializedPacket = serializer->Serialize(metaNode, defaultMap)->c_str();
				SXMPMeta fileMeta;
				fileMeta.ParseFromBuffer(serializedPacket.c_str(), serializedPacket.length());

				// Create a new XMP object from an RDF string
				//SXMPMeta rdfMeta = createXMPFromRDF();

				// Append the newly created properties onto the original XMP object
				// This will:
				// a) Add ANY new TOP LEVEL properties in the source (rdfMeta) to the destination (fileMeta)
				// b) Replace any top level properties in the source with the matching properties from the destination
				//SXMPUtils::ApplyTemplate(&fileMeta, rdfMeta, kXMPTemplate_AddNewProperties | kXMPTemplate_ReplaceExistingProperties | kXMPTemplate_IncludeInternalProperties);

				// Serialize the packet and write the buffer to a file
				// Let the padding be computed and use the default linefeed and indents without limits
				string newBuffer;
				fileMeta.SerializeToBuffer(&newBuffer);

				// Display the properties again to show changes
				cout << "After Appending Properties:" << endl;
				AdobeXMPCore::spIMetadata newMetaNode = parser->Parse(newBuffer.c_str(), newBuffer.size());
				displayPropertyValues(newMetaNode);

				// Write the packet to a file as RDF
				writeRDFToFile(&newBuffer, filename + "_XMP_RDF.txt");

				// Check we can put the XMP packet back into the file
				if (myFile.CanPutXMP(fileMeta))
				{
					// If so then update the file with the modified XMP
					myFile.PutXMP(fileMeta);
				}

				// Close the SXMPFile.  This *must* be called.  The XMP is not
				// actually written and the disk file is not closed until this call is made.
				myFile.CloseFile();
			}
			else
			{
				cout << "Unable to open " << filename << endl;
			}

		}
		catch (XMP_Error & e)
		{
			cout << "ERROR: " << e.GetErrMsg() << endl;
		}

		// Terminate the toolkit
		SXMPFiles::Terminate();
		SXMPMeta::Terminate();

	}
	else
	{
		cout << "Could not initialize SXMPFiles.";
		return -1;
	}

	return 0;
}


