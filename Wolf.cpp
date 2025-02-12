// License:
//
// Wolf
// Music indexing for SCI games
//
//
//
// 	(c) Jeroen P. Broks, 2022, 2023, 2025
//
// 		This program is free software: you can redistribute it and/or modify
// 		it under the terms of the GNU General Public License as published by
// 		the Free Software Foundation, either version 3 of the License, or
// 		(at your option) any later version.
//
// 		This program is distributed in the hope that it will be useful,
// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// 		GNU General Public License for more details.
// 		You should have received a copy of the GNU General Public License
// 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// 	Please note that some references to data like pictures or audio, do not automatically
// 	fall under this licenses. Mostly this is noted in the respective files.
//
// Version: 25.02.12 I
// End License

/* Orginal C# usings

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UseJCR6;
using TrickyUnits;

*/

#include <SlyvQCol.hpp>
#include <SlyvDirry.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvStream.hpp>
#include <SlyvConInput.hpp>

#include <JCR6_Core.hpp>
#include <JCR6_zlib.hpp>
#include <JCR6_JQL.hpp>

using namespace std;
using namespace Slyvina;
using namespace Units;
using namespace JCR6;


#define ProjectDir __ProjectDir()
#define ProjectFile __ProjectFile()
//#define Resource __Resource()
//#define Resource Ask("System", "Resource", "What JCR6 resource file should I use for this project?");
#define Resource Ask("Resources", Platform(), "What JCR6 resource file should I use for this project?")

#define lcase(a) } else if (outlang==a) {
#define ldefault } else {

// Wolf is a quick program that collects JukeBox data for an SCI project
// The name "Wolf" is derrived from WOLFgang Amadeaus Mozart
namespace NWolf {
	class Wolf {
	public:

		string Project;
		Wolf(string a) { Project = a; Data = LoadOptGINIE(ProjectFile, ProjectFile,"Project file for Wolf!"); }
		string ProjectDir { return Dirry("$AppSupport$/Wolf"); }
		string ProjectFile { return ProjectDir+"/"+Project+".ini"; }

		//string Resource { return Ask("System", "Resource", "What JCR6 resource file should I use for this project?"); }

		GINIE Data{nullptr};
		JT_Dir JRes{nullptr};
		map<string, vector<JT_Entry>> MusPos{};

		string Ask(string cat, string vr, string Question, string DefaultValue = "") {
			cat = Trim(cat);
			vr  = Trim(vr);
			DefaultValue =Trim(DefaultValue);
			while (Data->Value(cat, vr) == "") {
				if (DefaultValue != "") QCol->Magenta("["+DefaultValue+"] ");
				QCol->Yellow(Question+" ");
				QCol->Cyan("");
				Data->Value(cat, vr, ReadLine());
				if (Data->Value(cat, vr) == "") Data->Value(cat, vr, DefaultValue);
			}
			return Data->Value(cat, vr);
		}

		bool Yes(string cat, string vr, string Question) {
			while (Data->Value(cat, vr) == "") {
				QCol->Yellow(Question+" ");
				QCol->White("? ");
				QCol->Magenta("(Y/N) ");
				auto A = Upper(ReadLine()+"*"); //= Console.ReadKey(true);
				switch (A[0]) {
					case 'Y':
					case 'J':
						//QCol->Green("Yes\n");
						Data->Value(cat, vr, "YES");
						break;
					case 'N':
						//QCol->Red("No\n");
						Data->Value(cat, vr, "NO");
						break;
				}
			}
			return Upper(Data->Value(cat, vr)) == "YES";
		}


		string Used(vector<JT_Entry> EL) {
			if (EL.size() == 1) return EL[0]->Name();
			for (auto E : EL) {
				if (Upper(Data->Value("USED", E->Name())) == "YES") return E->Name();
			}
			return "";
		}

		void Analyze() {
			MusPos.clear();
			//MusPos = new SortedDictionary<string, List<TJCREntry>>();
			//for (var E in JRes.Entries.Values) {
			auto Entries{JRes->Entries()};
			for (auto E : *Entries) {
				if (ExtractExt(E->Name())!="" && Yes("ALLOWEXTENSION", ExtractExt(E->Name()), "Is '"+ExtractExt(E->Name()+"' a usable extension for WOLF in this project"))) {
					auto Place = E->MainFile+to_string(E->Offset());
					if (!MusPos.count(Place)) MusPos[Place] = vector<JT_Entry>(); //= new List<TJCREntry>();
					MusPos[Place].push_back(E);
				}
			}
		}

		void WorkOut() {
			vector<string> glist{};// = new List<string>();
			string output{""}; //= new StringBuilder();
			auto outlang { Ask("DataOut", "Language", "Output language ", "GINIE") };
			//switch (outlang)
			if (false) {
				lcase("NEIL") output += "\n#use \"Libs/LinkedList\"\n\nClass TMusic\n\tString Entry\n\tString Title\n\tString Artist\n\tString WebSite\n\tString License\n\tStatic Var Lst = New LinkedList()\nEnd\n\nInit\n"; //break;
				lcase("GINIE") output += ""; //break;
				ldefault QCol->Error("Unknown language"); return;
			}
			//foreach (var EL in MusPos.Values)
			for (auto& FEL:MusPos) {
				auto &EL{FEL.second};
				while (Used(EL) == "") {
					int ch = 0;
					//var items = new SortedDictionary<int, string>();
					map<int,string> items;
					for (auto E : EL)
						items[++ch] = E->Name();
					QCol->Yellow("The next entries share their mainfile and offset!\n");
					for (auto it : items) {
						QCol->Cyan(TrSPrintF("%4d> ", it.first));
						QCol->Green(it.second+"\n");
					}
					int choice = 0;
					do {
						QCol->Yellow("Which entry should I use? ");
						QCol->Cyan("");
						choice = ToInt(ReadLine());
					} while (!items.count(choice));
					Data->Value("Used", items[choice])== "YES";
				}
				auto F = Used(EL);
				auto D = EL[0];
				if (Yes("JukeBox", F, "Put '"+F+"' into the jukebox")) {
					//switch (outlang)
					if (false) {
						lcase("NEIL")
							output += "\tDo\n\t\tVar N = New TMusic()\n";
							output += "\t\tN.Entry   = \""+F+"\"\n";
							output += "\t\tN.License = \""+D->Notes()+"\"\n";
							output += "\t\tN.Title   = \""+Ask("Titles", F, "Title for '"+F+"': ", StripAll(F))+"\"\n";
							output += "\t\tN.Artist  = \""+Ask("Artists", D->Author(), "Artist name:", D->Author())+"\"\n";
							output += "\t\tN.WebSite = \""+Ask("Artist_site", D->Author(), D->Author()+"'s artist site:", "NONE")+"\"\n";
							output += "\t\tTMusic.Lst.AddLast(N)\n";
							output += "\tEnd\n";
							//break;
						lcase("GINIE")
							output += "[Entry:"+F+"]\n";
							output += "License="+D->Notes()+"\n";
							output += "Title="+ Ask("Titles", F, "Title for '"+F+"': ", StripAll(F))+"\n";
							output += "Artist="+Ask("Artists", D->Author(), "Artist name:", D->Author())+"\n";
							output += "WebSite="+Ask("Artist_site", D->Author(), D->Author()+"'s artist site:", "NONE")+"\n";
							//glist.Add(F);//output += "\t\tTMusic.Lst.AddLast(N)\n");
							glist.push_back(F);
							output += "\n\n";
							//break;
					}
				}
			}
			//switch (outlang)
			if (false) {
				lcase( "NEIL" )
					output += "End\n";
					//break;
				lcase( "GINIE")
					output += "[Index]\n*list:Index\n";
					for (auto E : glist) output += E+"\n";
					output += "*end\n\n";
					//break;
			}
			SaveString(Ask("System", "Output", "Script to output to: "), output);
		}

		void Run() {
			QCol->Doing("Project", Project);
			JRes = JCR6_Dir(Resource);
			QCol->Doing("Read", Resource);
			Analyze();
			WorkOut();
			QCol->Cyan("Ok\n");
			//Console.ResetColor();
			QCol->Reset();
		}

		static void Main(vector<string> args) {
			//Dirry.InitAltDrives();
			//MKL.Version("Wolf - Program.cs", "23.08.06");
			//MKL.Lic("Wolf - Program.cs", "GNU General Public License 3");
			//JCR6_lzma.Init();
			//JCR6_JXSDA.Init();
			//JCR6_zlib.Init();

			//new JCR_QuickLink();
			InitJQL();


			QCol->Yellow("Wolf (C++) " __DATE__ "; " __TIME__" \t");
			QCol->Magenta("Coded by: Tricky\n");
			QCol->Cyan("(c) Jeroen P. Broks 2022-20"+Right(__DATE__,2)+"\n\n");
			for (auto a : args) {
				//var w = new Wolf(a);
				Wolf w{a};
				w.Run();
			}
			//TrickyDebug.AttachWait();
		}
	};
}

int main(int c,char **a) {
	vector<string> ARGS{};
	for(int i=1;i<c;i++) ARGS.push_back(a[i]);
	NWolf::Wolf::Main(ARGS);
	return 0;
}

