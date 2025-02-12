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

#include <SlyvDirry.hpp>
#include <SlyvGINIE.hpp>

using namespace std;
using namespace Slyvina;
using namespace Units;


#define ProjectDir __ProjectDir()
#define ProjectFile __ProjectFile()
//#define Resource __Resource()
#define Resource Ask("System", "Resource", "What JCR6 resource file should I use for this project?");

// Wolf is a quick program that collects JukeBox data for an SCI project
// The name "Wolf" is derrived from WOLFgang Amadeaus Mozart
namespace NWolf {
	class Wolf {
	public:

		string Project;
		Wolf(string a) { Project = a; Data = GINIE.FromFile(ProjectFile); Data.AutoSaveSource = ProjectFile; }
		string ProjectDir {return Dirry("$AppSupport$/Wolf"); }
		string ProjectFile { return ProjectDir+"/"+Project+".ini"; }

		//string Resource { return Ask("System", "Resource", "What JCR6 resource file should I use for this project?"); }

		GINIE Data{nullptr};
		TJCRDIR JRes;
		SortedDictionary<string, List<TJCREntry>> MusPos;

		string Ask(string cat, string vr, string Question, string DefaultValue = "") {
			cat = cat.Trim();
			vr = vr.Trim();
			DefaultValue = DefaultValue.Trim();
			while (Data[cat, vr] == "") {
				if (DefaultValue != "") QCol.Magenta($"[{DefaultValue}] ");
				QCol.Yellow($"{Question} ");
				QCol.Cyan("");
				Data[cat, vr] = Console.ReadLine();
				if (Data[cat, vr] == "") Data[cat, vr] = DefaultValue;
			}
			return Data[cat, vr];
		}

		bool Yes(string cat, string vr, string Question) {
			while (Data[cat, vr] == "") {
				QCol.Yellow($"{Question} ");
				QCol.White("? ");
				QCol.Magenta("(Y/N) ");
				var A = Console.ReadKey(true);
				switch (A.Key) {
					case ConsoleKey.Y:
					case ConsoleKey.J:
						QCol.Green("Yes\n");
						Data[cat, vr] = "YES";
						break;
					case ConsoleKey.N:
						QCol.Red("No\n");
						Data[cat, vr] = "NO";
						break;
				}
			}
			return Data[cat, vr].ToUpper() == "YES";
		}


		string Used(List<TJCREntry> EL) {
			if (EL.Count == 1) return EL[0].Entry;
			foreach (var E in EL) {
				if (Data["USED", E.Entry].ToUpper() == "YES") return E.Entry;
			}
			return "";
		}

		void Analyze() {
			MusPos = new SortedDictionary<string, List<TJCREntry>>();
			foreach (var E in JRes.Entries.Values) {
				if (qstr.ExtractExt(E.Entry)!="" && Yes("ALLOWEXTENSION", qstr.ExtractExt(E.Entry), $"Is '{qstr.ExtractExt(E.Entry)}' a usable extension for WOLF in this project")) {
					var Place = $"{E.MainFile}::{E.Offset}";
					if (!MusPos.ContainsKey(Place)) MusPos[Place] = new List<TJCREntry>();
					MusPos[Place].Add(E);
				}
			}
		}

		void WorkOut() {
			var glist = new List<string>();
			var output = new StringBuilder();
			var outlang = Ask("DataOut", "Language", "Output language ", "Neil").ToUpper();
			switch (outlang) {
				case "NEIL": output.Append("\n#use \"Libs/LinkedList\"\n\nClass TMusic\n\tString Entry\n\tString Title\n\tString Artist\n\tString WebSite\n\tString License\n\tStatic Var Lst = New LinkedList()\nEnd\n\nInit\n"); break;
				case "GINIE": output.Append(""); break;
				default: QCol.QuickError("Unknown language"); return;
			}
			foreach (var EL in MusPos.Values) {
				while (Used(EL) == "") {
					int ch = 0;
					var items = new SortedDictionary<int, string>();
					foreach (var E in EL)
						items[++ch] = E.Entry;
					QCol.Yellow("The next entries share their mainfile and offset!\n");
					foreach (var it in items) {
						QCol.Cyan(Fmt.sprintf("%4d> ", it.Key));
						QCol.Green($"{it.Value}\n");
					}
					int choice = 0;
					do {
						QCol.Yellow("Which entry should I use? ");
						QCol.Cyan("");
						choice = qstr.ToInt(Console.ReadLine());
					} while (!items.ContainsKey(choice));
					Data["Used", items[choice]] = "YES";
				}
				var F = Used(EL);
				var D = EL[0];
				if (Yes("JukeBox", F, $"Put {F} into the jukebox")) {
					switch (outlang) {
						case "NEIL":
							output.Append("\tDo\n\t\tVar N = New TMusic()\n");
							output.Append($"\t\tN.Entry   = \"{F}\"\n");
							output.Append($"\t\tN.License = \"{D.Notes}\"\n");
							output.Append($"\t\tN.Title   = \"{Ask("Titles", F, $"Title for '{F}': ", qstr.StripAll(F))}\"\n");
							output.Append($"\t\tN.Artist  = \"{Ask("Artists", D.Author, "Artist name:", D.Author)}\"\n");
							output.Append($"\t\tN.WebSite = \"{Ask("Artist_site", D.Author, $"{D.Author}'s artist site:", "NONE")}\"\n");
							output.Append("\t\tTMusic.Lst.AddLast(N)\n");
							output.Append("\tEnd\n");
							break;
						case "GINIE":
							output.Append($"[Entry:{F}]\n");
							output.Append($"License={D.Notes}\n");
							output.Append($"Title={ Ask("Titles", F, $"Title for '{F}': ", qstr.StripAll(F))}\n");
							output.Append($"Artist={Ask("Artists", D.Author, "Artist name:", D.Author)}\n");
							output.Append($"WebSite={Ask("Artist_site", D.Author, $"{D.Author}'s artist site:", "NONE")}\n");
							glist.Add(F);//output.Append("\t\tTMusic.Lst.AddLast(N)\n");
							output.Append("\n\n");
							break;
					}
				}
			}
			switch (outlang) {
				case "NEIL":
					output.Append("End\n");
					break;
				case "GINIE":
					output.Append("[Index]\n*list:Index\n");
					foreach (var E in glist) output.Append($"{E}\n");
					output.Append("*end\n\n");
					break;
			}
			QuickStream.SaveString(Ask("System", "Output", "Script to output to: "), output);
		}

		void Run() {
			QCol.Doing("Project", Project);
			JRes = JCR6.Dir(Resource);
			QCol.Doing("Read", Resource);
			Analyze();
			WorkOut();
			QCol.Cyan("Ok\n");
			Console.ResetColor();
		}

		static void Main(vector<string> args) {
			Dirry.InitAltDrives();
			MKL.Version("Wolf - Program.cs", "23.08.06");
			MKL.Lic("Wolf - Program.cs", "GNU General Public License 3");
			JCR6_lzma.Init();
			JCR6_JXSDA.Init();
			JCR6_zlib.Init();
			new JCR_QuickLink();

			QCol.Yellow($"Wolf {MKL.Newest}\t");
			QCol.Magenta("Coded by: Tricky\n");
			QCol.Cyan($"(c) Jeroen P. Broks 2022-20{qstr.Left(MKL.Newest, 2)}\n\n");
			foreach (var a in args) {
				var w = new Wolf(a);
				w.Run();
			}
			TrickyDebug.AttachWait();
		}
	}
}
int main(int c,char **a){
	vector<string> ARGS{}
	for(int i=1;i<c;i++) ARGS.push_back(a[i]);
	NWolf::Wolf::Main(ARGS);
	return 0;
}
