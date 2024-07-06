using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using Microsoft.VisualBasic;

namespace PPL2OpenPPL_CS
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }

        private void btnExit_Click(System.Object sender, System.EventArgs e)
        {
            this.Close();
        }

        private void btnFileSelection_Click(System.Object sender, System.EventArgs e)
        {
            string path = txtFile.Text.Trim();
            ofd.Filter = "All files (*.*)|*.*";
            ofd.FileName = "";
            if (path.Length == 0)
                ofd.InitialDirectory = Application.StartupPath;
            else
                ofd.InitialDirectory = path.Substring(0, path.LastIndexOf(@"\"));
            if (ofd.ShowDialog() == DialogResult.OK)
                txtFile.Text = ofd.FileName;
        }

        private void btnRun_Click(System.Object sender, System.EventArgs e)
        {
            try
            {
                txtFile.Text = txtFile.Text.Trim();
                string[] files = Directory.GetFiles(txtFile.Text.Substring(0, txtFile.Text.LastIndexOf(@"\")), txtFile.Text.Substring(txtFile.Text.LastIndexOf(@"\") + 1));
                pb.Maximum = files.Length;
                pb.Value = 0;
                foreach (string inputFile in files)
                {
                    lblFile.Text = inputFile.Substring(inputFile.LastIndexOf(@"\") + 1);
                    pb.Value++;
                    Application.DoEvents();
                    string outputFile = inputFile + ".oppl";
                    if (File.Exists(outputFile))
                        File.Delete(outputFile);
                    lb.Items.Clear();

                    outputLines = new Dictionary<int, string>();
                    string[] inputLines = File.ReadAllLines(inputFile);
                    loadTables();

                    int i = 0;
                    string l = "";
                    bool preflopFound = false;
                    while (i < inputLines.Length)
                    {
                        inputLines[i] = inputLines[i].Trim();
                        inputLines[i] = inputLines[i].Replace("\n", "").Replace("\t", "");

                        // Empty line
                        if (inputLines[i].Length != 0)
                        {
                            if (inputLines[i].StartsWith("//")) // Comment line
                            {
                                if (l.Length > 0)
                                {
                                    completeLineProcess(ref l);
                                    lineAdd(l);
                                }
                                lineAdd(inputLines[i]);
                                l = "";
                            }
                            else
                            {
                                // We remove comments at the end of the other lines (don't how to deal with it)
                                if (inputLines[i].Contains("//"))
                                    inputLines[i] = inputLines[i].Substring(0, inputLines[i].IndexOf("//")).Trim();

                                string firstWord = inputLines[i].ToLower().Split(' ')[0];

                                // Section line
                                if (",preflop,flop,turn,river,".Contains("," + firstWord + ","))
                                {
                                    preflopFound = true;
                                    if (l.Length > 0)
                                    {
                                        completeLineProcess(ref l);
                                        lineAdd(l);
                                    }
                                    lineAdd("##f$" + firstWord + "##");
                                    l = "";
                                }
                                else if (firstWord == "when") // New real line
                                {
                                    if (l.Length > 0)
                                    {
                                        completeLineProcess(ref l);
                                        lineAdd(l);
                                    }
                                    l = inputLines[i].ToLower();
                                }
                                else if (preflopFound) // Adding line to previous one
                                    l += " " + inputLines[i].ToLower();
                            }
                        }
                        i++;
                    }
                    if (l.Length > 0)
                    {
                        completeLineProcess(ref l);
                        lineAdd(l);
                    }

                    // Saving the result formated way
                    for (int j = 0; j < outputLines.Count; j++)
                    {
                        try
                        {
                            outputLines[j] = outputLines[j].Replace("\n", "").Replace("\t", "");
                            if (outputLines[j].ToLower().Contains("##f$"))
                                outputLines[j] = "\n" + outputLines[j] + "\n\n";
                            else if (outputLines[j].StartsWith("//"))
                                outputLines[j] = outputLines[j] + "\n";
                            else if (outputLines[j].ToLower().EndsWith("force") || outputLines[j].ToLower().Substring(outputLines[j].ToLower().LastIndexOf(" ") - 3).StartsWith("set user"))
                                outputLines[j] = "\t" + outputLines[j] + "\n";
                            else
                                outputLines[j] = "\n" + outputLines[j] + "\n";
                            File.AppendAllText(outputFile, outputLines[j]);
                        }
                        catch (Exception ex)
                        {
                            File.AppendAllText(outputFile, "\n\nERROR: " + ex.Message + "\n\n" + outputLines[j] + "\n\n");
                        }
                    }
                }

                MessageBox.Show("Process finished");
            }
            catch (Exception ex)
            {
                MessageBox.Show("ERROR: " + ex.Message);
            }
        }

        private void completeLineProcess(ref string line)
        {
            string oldLine = "";
            delaySolve(ref line);
            spacesSolve(ref line, new string[2] { "in s|ins", "in b|inb" });
            specialCharsProcess(ref line);
            while (line != oldLine)
            {
                oldLine = line;
                lineProcess(ref line);
            }
            spacesSolve(ref line, new string[3] { "hand = |hand$", "board = |board$", " Suited|Suited" });
            handSolve(ref line);
            spacesSolve(ref line, new string[5] { " . |.", "! =|!=", "< =|<=", "> =|>=", " %|%" });
            endForceSolve(ref line);
            raiseForceSolve(ref line);
            betSizeSolve(ref line);
        }

        private void endForceSolve(ref string line)
        {
            if (line.Replace("\n", "").Replace("\t", "").TrimEnd().ToLower().EndsWith("force")) return;
            string[] l = line.ToLower().Trim().Split(' ');
            int m = l.Length;
            foreach (string c in new string[7] { "beep", "call", "play", "raise", "fold", "bet", "sitout" })
            {
                if (l[m - 1].StartsWith(c) && !"<=>=%andornot".Contains(l[m - 2]))
                    line += " FORCE";
                else if (c == "raise" && l[m - 2].StartsWith(c) && IsNumeric(l[m - 1]))
                    line += " FORCE";
            }
        }

        const string specialChars = "<>=%()!.";
        private void specialCharsProcess(ref string line)
        {
            string res = "";
            for (int i = 0; i <= line.Length - 1; i++)
            {
                if (specialChars.Contains("" + line[i]))
                    res += " ";
                res += line[i];
                if (specialChars.Contains("" + line[i]))
                    res += " ";
            }
            line = res;
        }

        private void lineProcess(ref string line)
        {
            string[] words = line.Trim().Split(' ');
            line = "";
            foreach (string word in words)
            {
                string w = word.Replace("2nd", "second").Replace("3rd", "third").Replace("4th", "fourth").Replace("5th", "fifth");
                if (keysTable.ContainsKey(w))
                    line += " " + keysTable[w];
                else if (varsTable.ContainsKey(w.ToLower()))
                    line += " " + varsTable[w.ToLower()];
                else
                    line += " " + w;
            }
            line = line.Substring(1);
        }

        private void betSizeSolve(ref string line)
        {
            foreach (string c in new string[3] { "BetSize", "StackSize", "AmountToCall" })
                line = line.Replace(c + " =", c + " ~~");
        }

        private void raiseForceSolve(ref string line)
        {
            string[] words = line.Trim().Split(' ');
            if (words[words.Length - 1] == "FORCE")
            {
                if (words[words.Length - 3] == "Raise" && (Information.IsNumeric(words[words.Length - 2]) || words[words.Length - 2].EndsWith("%")))
                    line = line.Substring(0, line.LastIndexOf("Raise")) + "RaiseBy" + line.Substring(line.LastIndexOf("Raise") + 5);
                else if (words[words.Length - 3] == "Bet" && (Information.IsNumeric(words[words.Length - 2]) || words[words.Length - 2].EndsWith("%")))
                    line = line.Substring(0, line.LastIndexOf("Bet")) + "RaiseBy" + line.Substring(line.LastIndexOf("Bet") + 3);
                else if (words[words.Length - 2] == "Bet" && words[words.Length - 1] == "FORCE")
                    line = line.Substring(0, line.LastIndexOf("Bet")) + "Raise" + line.Substring(line.LastIndexOf("Bet") + 3);
            }
        }

        private void delaySolve(ref string line)
        {
            int i = line.LastIndexOf("delay ");
            if (i > 0)
                line = line.Substring(0, i).Trim();
        }

        private void spacesSolve(ref string line, string[] changes)
        {
            line = line.Trim();
            for (int i = 10; i >= 2; i += -1)
                line = line.Replace(" ".PadLeft(i), " ");
            foreach (string c in changes)
            {
                string[] d = c.Split('|');
                line = line.Replace(d[0], d[1]);
            }
        }

        private void handSolve(ref string line)
        {
            string[] words = line.Split(' ');
            string res = "";
            bool bSee = false;
            foreach (string word in words)
            {
                if (word.Trim().Length == 0)
                    continue;
                if (word.StartsWith("hand$") || word.StartsWith("board$"))
                {
                    res += " " + word;
                    bSee = true;
                }
                else if (bSee)
                {
                    if ((word.Length < 3 || word.EndsWith("Suited")) && !(word == "OR" || word == ")"))
                        res += word;
                    else
                    {
                        res += " " + word;
                        bSee = false;
                    }
                }
                else
                    res += " " + word;
            }
            line = res;
        }

        Dictionary<int, string> outputLines;
        private void lineAdd(string line)
        {
            outputLines.Add(outputLines.Count, line);
            lb.Items.Add(line);
            lb.SelectedIndex = lb.Items.Count - 1;
            Application.DoEvents();
        }

        Dictionary<string, string> keysTable = new Dictionary<string, string>();
        Dictionary<string, string> varsTable = new Dictionary<string, string>();
        private void loadTables()
        {
            string inputFile = Application.StartupPath + "\\keys.txt";
            if (!File.Exists(inputFile))
                throw new Exception("File " + inputFile + " does not exists");
            keysTable.Clear();
            foreach (string l in File.ReadAllLines(inputFile))
            {
                string[] v = l.Split(',');
                keysTable.Add(v[0].ToLower(), v[1]);
            }
            inputFile = Application.StartupPath + "\\OpenPPL_Library.ohf";
            if (!File.Exists(inputFile))
                throw new Exception("File " + inputFile + " does not exists");
            varsTable.Clear();
            foreach (string line in File.ReadAllLines(inputFile))
            {
                string l = line;
                l = l.Trim();
                if (l.StartsWith("##") && l.EndsWith("##"))
                {
                    l = l.Replace("##", "").Replace("f$", "");
                    if (!varsTable.ContainsKey(l.ToLower()))
                        varsTable.Add(l.ToLower(), l);
                }
            }
        }

        private bool IsNumeric(Object Expression)
        {
            if (Expression.ToString().EndsWith("%")) return true;

            if (Expression == null || Expression is DateTime)
                return false;

            if (Expression is Int16 || Expression is Int32 || Expression is Int64 || Expression is Decimal || Expression is Single || Expression is Double || Expression is Boolean)
                return true;

            try
            {
                if (Expression is string)
                    Double.Parse(Expression as string);
                else
                    Double.Parse(Expression.ToString());
                return true;
            }
            catch { } // just dismiss errors but return false
            return false;
        }
    }
}
