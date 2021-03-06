/*
 * Copyright 2006 the original author or authors.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <sstream>
#include <cctype>
#include "GenException.h"
#include "Util.h"
#include "ElmtFactory.h"
#include "Configuration.h"
#include "HeadFile.h"

using namespace std;

HeadFile::HeadFile(string infile)
{
	this->filename = infile;
	this->Basename = Util::basenameOf(infile);
	if( Configuration::getOutPath().empty() )
		this->OutPath = Util::dirOf(infile);
	else
		this->OutPath = Configuration::getOutPath();

	string fileContent, rest;
	this->readFile(infile, fileContent);

	int idx = 0, ridx;
	DocCommentElmt* dc = NULL;
	while( idx <fileContent.size() ){
		rest = fileContent.substr(idx);

		if( ridx = Util::irrelevantLen(rest) ){
			idx += ridx;
		}
		else{
			IElement* e = ElmtFactory::makeElmt(fileContent, idx);
			if( e == NULL )
				idx += Util::unrecognisedLen(rest);
			else{
				this->Elements.push_back(e);

				// if e is a document comment
				if( e->getType() == IElement::DOCCOMMENT ){
					dc = (DocCommentElmt*)e;
				}
				else if( dc != NULL ) {
					e->associateComment(dc);
					dc = NULL;
				}
			}
		}
	}
}

HeadFile::~HeadFile()
{
	for( int i=0; i<this->Elements.size(); i++)
		delete this->Elements[i];
}

void HeadFile::genWrapper()
{
	if( 0 == this->genWrapperH() )
		this->genWrapperCPP();
}

int HeadFile::genWrapperH()
{
	string filebase = this->Basename + Configuration::getFWS();
	string outfile;
	
	if( !this->OutPath.empty() )
		outfile = this->OutPath + "/";
	outfile += filebase + Configuration::getHFS();

	string ws;
	// only generate namespace and class
	for( int i=0; i<this->Elements.size(); i++){
		IElement* e = this->Elements[i];
		if( e->getType() == IElement::NAMESAPCE ||
				e->getType() == IElement::CLASS ){
			ws += e->genWrapperH();
		}
	}

	if( Util::trimall(ws).empty() ) {
		// is not a error.
		Util::outputMessage("has nothing to generate for file: " + 
				this->filename);
		return -1;
	}

	ofstream wf(outfile.c_str());
	if( !wf.is_open() ){
		throw GenException("Can't open file to write: " + outfile);
	}

	wf << this->licenseInfo();
	wf << "#ifndef " << Util::toUpper(filebase) << "_H" << endl;
	wf << "#define " << Util::toUpper(filebase) << "_H" << endl;
	wf << endl;
	wf << "#include \"IBeanWrapper.h\"" << endl;
	wf << "#include \"" << this->Basename << ".h\"" << endl;
	wf << endl;
	wf << ws;
	wf << "#endif";
	wf << endl;
	wf.close();

	return 0;
}

void HeadFile::genWrapperCPP()
{
	string filebase = this->Basename + Configuration::getFWS();
	string outfile;
	
	if( !this->OutPath.empty() )
		outfile = this->OutPath + "/";
	outfile += filebase + Configuration::getIFS();

	string ws;
	// only generate namespace and class
	for( int i=0; i<this->Elements.size(); i++){
		IElement* e = this->Elements[i];
		if( e->getType() == IElement::NAMESAPCE ||
				e->getType() == IElement::CLASS ){
			ws += e->genWrapperCPP();
		}
	}
	
	// if has generated a .h file, continue to generate the .cpp file.
	// So, doesn't judge ws is empty or not.

	ofstream wf(outfile.c_str());
	if( !wf.is_open() ){
		throw GenException("Can't open file to write: " + outfile);
	}

	wf << this->licenseInfo();
	wf << "#include \"" << filebase << ".h\"" << endl;
	wf << endl;
	wf << ws;
	wf.close();
}

void HeadFile::readFile(string infile, string& content)
{
	ifstream hf(infile.c_str());
	if( !hf.is_open() ){
		throw GenException("Can't open file to read: " + infile);
	}

	string s;
	while( getline(hf, s) ){
		content = content + s + "\n";
	}

	hf.close();
}

string HeadFile::licenseInfo()
{
	ostringstream s;
	s<<
	"/*"																<<endl<<
	" * Generated by Autumn Generator."									<<endl<<
	" * "																<<endl<<
	" * Copyright 2006 the original author or authors."					<<endl<<
	" * "																<<endl<<
	" * Licensed under the Apache License, Version 2.0 (the \"License\");" <<endl<<
	" * you may not use this file except in compliance with the License." <<endl<<
	" * You may obtain a copy of the License at"						<<endl<<
	" * "																<<endl<<
	" *      http://www.apache.org/licenses/LICENSE-2.0"				<<endl<<
	" * "																<<endl<<
	" * Unless required by applicable law or agreed to in writing, software" <<endl<<
	" * distributed under the License is distributed on an \"AS IS\" BASIS," <<endl<<
	" * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied." <<endl<<
	" * See the License for the specific language governing permissions and" <<endl<<
	" * limitations under the License."									<<endl<<
	" */"																<<endl<<
																		  endl;
	
	return s.str();
}
