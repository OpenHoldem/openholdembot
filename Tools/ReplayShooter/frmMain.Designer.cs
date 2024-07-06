//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

namespace ReplayShooter
{
    partial class FrmMain
    {
        /// <summary>
        /// Variable del diseñador requerida.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén utilizando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben eliminar; false en caso contrario.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido del método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmMain));
            this.btnCapture = new System.Windows.Forms.Button();
            this.btnExit = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.txtOutputFolder = new System.Windows.Forms.TextBox();
            this.btnChooseOutputDir = new System.Windows.Forms.Button();
            this.fbDir = new System.Windows.Forms.FolderBrowserDialog();
            this.label2 = new System.Windows.Forms.Label();
            this.nFiles = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.cmbWindows = new System.Windows.Forms.ComboBox();
            this.btnWindowsRefresh = new System.Windows.Forms.Button();
            this.lblStatus = new System.Windows.Forms.Label();
            this.nDelay = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.nFiles)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nDelay)).BeginInit();
            this.SuspendLayout();
            // 
            // btnCapture
            // 
            this.btnCapture.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCapture.Location = new System.Drawing.Point(12, 99);
            this.btnCapture.Name = "btnCapture";
            this.btnCapture.Size = new System.Drawing.Size(75, 23);
            this.btnCapture.TabIndex = 6;
            this.btnCapture.Text = "Start";
            this.btnCapture.UseVisualStyleBackColor = true;
            this.btnCapture.Click += new System.EventHandler(this.btnCapture_Click);
            // 
            // btnExit
            // 
            this.btnExit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnExit.Location = new System.Drawing.Point(605, 99);
            this.btnExit.Name = "btnExit";
            this.btnExit.Size = new System.Drawing.Size(75, 23);
            this.btnExit.TabIndex = 7;
            this.btnExit.Text = "Exit";
            this.btnExit.UseVisualStyleBackColor = true;
            this.btnExit.Click += new System.EventHandler(this.btnExit_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Output folder:";
            // 
            // txtOutputFolder
            // 
            this.txtOutputFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtOutputFolder.Location = new System.Drawing.Point(122, 10);
            this.txtOutputFolder.Name = "txtOutputFolder";
            this.txtOutputFolder.Size = new System.Drawing.Size(525, 20);
            this.txtOutputFolder.TabIndex = 0;
            // 
            // btnChooseOutputDir
            // 
            this.btnChooseOutputDir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnChooseOutputDir.Location = new System.Drawing.Point(653, 8);
            this.btnChooseOutputDir.Name = "btnChooseOutputDir";
            this.btnChooseOutputDir.Size = new System.Drawing.Size(27, 23);
            this.btnChooseOutputDir.TabIndex = 1;
            this.btnChooseOutputDir.Text = "...";
            this.btnChooseOutputDir.UseVisualStyleBackColor = true;
            this.btnChooseOutputDir.Click += new System.EventHandler(this.btnChooseOutputDir_Click);
            // 
            // fbDir
            // 
            this.fbDir.RootFolder = System.Environment.SpecialFolder.MyComputer;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(87, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Max. output files:";
            // 
            // nFiles
            // 
            this.nFiles.Location = new System.Drawing.Point(122, 38);
            this.nFiles.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.nFiles.Name = "nFiles";
            this.nFiles.Size = new System.Drawing.Size(66, 20);
            this.nFiles.TabIndex = 2;
            this.nFiles.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 67);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(100, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Window to capture:";
            // 
            // cmbWindows
            // 
            this.cmbWindows.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbWindows.FormattingEnabled = true;
            this.cmbWindows.Location = new System.Drawing.Point(120, 64);
            this.cmbWindows.Name = "cmbWindows";
            this.cmbWindows.Size = new System.Drawing.Size(527, 21);
            this.cmbWindows.Sorted = true;
            this.cmbWindows.TabIndex = 4;
            // 
            // btnWindowsRefresh
            // 
            this.btnWindowsRefresh.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnWindowsRefresh.Location = new System.Drawing.Point(653, 62);
            this.btnWindowsRefresh.Name = "btnWindowsRefresh";
            this.btnWindowsRefresh.Size = new System.Drawing.Size(27, 23);
            this.btnWindowsRefresh.TabIndex = 5;
            this.btnWindowsRefresh.Text = "R";
            this.btnWindowsRefresh.UseVisualStyleBackColor = true;
            this.btnWindowsRefresh.Click += new System.EventHandler(this.btnWindowsRefresh_Click);
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.ForeColor = System.Drawing.Color.Red;
            this.lblStatus.Location = new System.Drawing.Point(93, 157);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(32, 13);
            this.lblStatus.TabIndex = 11;
            this.lblStatus.Text = "Error:";
            this.lblStatus.Visible = false;
            // 
            // nDelay
            // 
            this.nDelay.Location = new System.Drawing.Point(581, 38);
            this.nDelay.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.nDelay.Minimum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.nDelay.Name = "nDelay";
            this.nDelay.Size = new System.Drawing.Size(66, 20);
            this.nDelay.TabIndex = 3;
            this.nDelay.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(458, 40);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(117, 13);
            this.label4.TabIndex = 12;
            this.label4.Text = "Delay between images:";
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnExit;
            this.ClientSize = new System.Drawing.Size(692, 134);
            this.Controls.Add(this.nDelay);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.btnWindowsRefresh);
            this.Controls.Add(this.cmbWindows);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.nFiles);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btnChooseOutputDir);
            this.Controls.Add(this.txtOutputFolder);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnExit);
            this.Controls.Add(this.btnCapture);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "FrmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "OpenReplayShooter";
            ((System.ComponentModel.ISupportInitialize)(this.nFiles)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nDelay)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnCapture;
        private System.Windows.Forms.Button btnExit;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtOutputFolder;
        private System.Windows.Forms.Button btnChooseOutputDir;
        private System.Windows.Forms.FolderBrowserDialog fbDir;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown nFiles;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cmbWindows;
        private System.Windows.Forms.Button btnWindowsRefresh;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.NumericUpDown nDelay;
        private System.Windows.Forms.Label label4;
    }
}

