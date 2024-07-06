namespace PPL2OpenPPL_CS
{
    partial class frmMain
    {
        /// <summary>
        /// Variable del diseñador requerida.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén utilizando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben eliminar; false en caso contrario, false.</param>
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.btnExit = new System.Windows.Forms.Button();
            this.btnRun = new System.Windows.Forms.Button();
            this.lb = new System.Windows.Forms.ListBox();
            this.Label2 = new System.Windows.Forms.Label();
            this.btnFileSelect = new System.Windows.Forms.Button();
            this.txtFile = new System.Windows.Forms.TextBox();
            this.Label1 = new System.Windows.Forms.Label();
            this.ofd = new System.Windows.Forms.OpenFileDialog();
            this.pb = new System.Windows.Forms.ProgressBar();
            this.lblFile = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btnExit
            // 
            this.btnExit.Location = new System.Drawing.Point(613, 488);
            this.btnExit.Name = "btnExit";
            this.btnExit.Size = new System.Drawing.Size(75, 44);
            this.btnExit.TabIndex = 13;
            this.btnExit.Text = "Exit";
            this.btnExit.UseVisualStyleBackColor = true;
            this.btnExit.Click += new System.EventHandler(this.btnExit_Click);
            // 
            // btnRun
            // 
            this.btnRun.Location = new System.Drawing.Point(12, 488);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(75, 44);
            this.btnRun.TabIndex = 12;
            this.btnRun.Text = "Run";
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Click += new System.EventHandler(this.btnRun_Click);
            // 
            // lb
            // 
            this.lb.FormattingEnabled = true;
            this.lb.Location = new System.Drawing.Point(12, 62);
            this.lb.Name = "lb";
            this.lb.Size = new System.Drawing.Size(676, 420);
            this.lb.TabIndex = 11;
            // 
            // Label2
            // 
            this.Label2.AutoSize = true;
            this.Label2.Location = new System.Drawing.Point(12, 46);
            this.Label2.Name = "Label2";
            this.Label2.Size = new System.Drawing.Size(59, 15);
            this.Label2.TabIndex = 10;
            this.Label2.Text = "Progress:";
            // 
            // btnFileSelect
            // 
            this.btnFileSelect.Location = new System.Drawing.Point(661, 10);
            this.btnFileSelect.Name = "btnFileSelect";
            this.btnFileSelect.Size = new System.Drawing.Size(28, 23);
            this.btnFileSelect.TabIndex = 9;
            this.btnFileSelect.Text = "...";
            this.btnFileSelect.UseVisualStyleBackColor = true;
            this.btnFileSelect.Click += new System.EventHandler(this.btnFileSelection_Click);
            // 
            // txtFile
            // 
            this.txtFile.Location = new System.Drawing.Point(103, 12);
            this.txtFile.Name = "txtFile";
            this.txtFile.Size = new System.Drawing.Size(552, 20);
            this.txtFile.TabIndex = 8;
            // 
            // Label1
            // 
            this.Label1.AutoSize = true;
            this.Label1.Location = new System.Drawing.Point(12, 15);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(85, 15);
            this.Label1.TabIndex = 7;
            this.Label1.Text = "File to convert:";
            // 
            // ofd
            // 
            this.ofd.FileName = "openFileDialog1";
            // 
            // pb
            // 
            this.pb.Location = new System.Drawing.Point(94, 507);
            this.pb.Name = "pb";
            this.pb.Size = new System.Drawing.Size(513, 25);
            this.pb.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.pb.TabIndex = 14;
            // 
            // lblFile
            // 
            this.lblFile.ForeColor = System.Drawing.Color.Red;
            this.lblFile.Location = new System.Drawing.Point(93, 489);
            this.lblFile.Name = "lblFile";
            this.lblFile.Size = new System.Drawing.Size(514, 15);
            this.lblFile.TabIndex = 15;
            this.lblFile.Text = " ";
            this.lblFile.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(695, 539);
            this.Controls.Add(this.lblFile);
            this.Controls.Add(this.pb);
            this.Controls.Add(this.btnExit);
            this.Controls.Add(this.btnRun);
            this.Controls.Add(this.lb);
            this.Controls.Add(this.Label2);
            this.Controls.Add(this.btnFileSelect);
            this.Controls.Add(this.txtFile);
            this.Controls.Add(this.Label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "frmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "PPL 2 OpenPPL";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        internal System.Windows.Forms.Button btnExit;
        internal System.Windows.Forms.Button btnRun;
        internal System.Windows.Forms.ListBox lb;
        internal System.Windows.Forms.Label Label2;
        internal System.Windows.Forms.Button btnFileSelect;
        internal System.Windows.Forms.TextBox txtFile;
        internal System.Windows.Forms.Label Label1;
        private System.Windows.Forms.OpenFileDialog ofd;
        private System.Windows.Forms.ProgressBar pb;
        internal System.Windows.Forms.Label lblFile;
    }
}

