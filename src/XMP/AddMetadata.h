#pragma once
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string 

// Must be defined to give access to XMPFiles
#define XMP_INCLUDE_XMPFILES 1 

#define ENABLE_NEW_DOM_MODEL 1

// Ensure XMP templates are instantiated
#include "XMP.incl_cpp"

// Provide access to the API
#include "XMP.hpp"

#include "XMPCore/Interfaces/IDOMImplementationRegistry.h"
#include "XMPCore/Interfaces/IDOMParser.h"
#include "XMPCore/Interfaces/IDOMSerializer.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"
#include "XMPCore/Interfaces/ISimpleNode.h"
#include "XMPCore/Interfaces/IStructureNode.h"
#include "XMPCore/Interfaces/IArrayNode.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/INodeIterator.h"

using namespace std;
//using namespace AdobeXMPCore;

void GetLocalizedText(AdobeXMPCore::spIArrayNode, const char*, const char* , string );
void displayPropertyValues(AdobeXMPCore::spIMetadata metaNode);
SXMPMeta createXMPFromRDF();
void writeRDFToFile(string * rdf, string filename);
int addXMP(string, std::vector<std::string>);
