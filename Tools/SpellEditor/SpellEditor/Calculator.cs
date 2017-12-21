using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SpellEditor
{
    public partial class CalculatorForm : Form
    {
        public CalculatorForm()
        {
            InitializeComponent();
        }
        int Result = 0;
        private void CalculatorForm_Load(object sender, EventArgs e)
        {
            MaskReslut.ReadOnly = true;
            MaskReslut.Text = int.MaxValue.ToString();
        }

        private void Mask1_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask1.Checked)
            {
                Result |= 1 << 0;
            }
            else
            {
                int temp = 1 << 0;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask2_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask2.Checked)
            {
                Result |= 1 << 1;
            }
            else
            {
                int temp = 1 << 1;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask3_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask3.Checked)
            {
                Result |= 1 << 2;
            }
            else
            {
                int temp = 1 << 2;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask4_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask4.Checked)
            {
                Result |= 1 << 3;
            }
            else
            {
                int temp = 1 << 3;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask5_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask5.Checked)
            {
                Result |= 1 << 4;
            }
            else
            {
                int temp = 1 << 4;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask6_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask6.Checked)
            {
                Result |= 1 << 5;
            }
            else
            {
                int temp = 1 << 5;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask7_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask7.Checked)
            {
                Result |= 1 << 6;
            }
            else
            {
                int temp = 1 << 6;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask8_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask8.Checked)
            {
                Result |= 1 << 7;
            }
            else
            {
                int temp = 1 << 7;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask9_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask9.Checked)
            {
                Result |= 1 << 8;
            }
            else
            {
                int temp = 1 << 8;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask10_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask10.Checked)
            {
                Result |= 1 << 9;
            }
            else
            {
                int temp = 1 << 9;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask11_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask11.Checked)
            {
                Result |= 1 << 10;
            }
            else
            {
                int temp = 1 << 10;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask12_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask12.Checked)
            {
                Result |= 1 << 11;
            }
            else
            {
                int temp = 1 << 11;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask13_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask13.Checked)
            {
                Result |= 1 << 12;
            }
            else
            {
                int temp = 1 << 12;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask14_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask14.Checked)
            {
                Result |= 1 << 13;
            }
            else
            {
                int temp = 1 << 13;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask15_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask15.Checked)
            {
                Result |= 1 << 14;
            }
            else
            {
                int temp = 1 << 14;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask16_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask16.Checked)
            {
                Result |= 1 << 15;
            }
            else
            {
                int temp = 1 << 15;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask17_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask17.Checked)
            {
                Result |= 1 << 16;
            }
            else
            {
                int temp = 1 << 16;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask18_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask18.Checked)
            {
                Result |= 1 << 17;
            }
            else
            {
                int temp = 1 << 17;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask19_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask19.Checked)
            {
                Result |= 1 << 18;
            }
            else
            {
                int temp = 1 << 18;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask20_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask20.Checked)
            {
                Result |= 1 << 19;
            }
            else
            {
                int temp = 1 << 19;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask21_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask21.Checked)
            {
                Result |= 1 << 20;
            }
            else
            {
                int temp = 1 << 20;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask22_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask22.Checked)
            {
                Result |= 1 << 21;
            }
            else
            {
                int temp = 1 << 21;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask23_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask23.Checked)
            {
                Result |= 1 << 22;
            }
            else
            {
                int temp = 1 << 22;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask24_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask24.Checked)
            {
                Result |= 1 << 23;
            }
            else
            {
                int temp = 1 << 23;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask25_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask25.Checked)
            {
                Result |= 1 << 24;
            }
            else
            {
                int temp = 1 << 24;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask26_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask26.Checked)
            {
                Result |= 1 << 25;
            }
            else
            {
                int temp = 1 << 25;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask27_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask27.Checked)
            {
                Result |= 1 << 26;
            }
            else
            {
                int temp = 1 << 26;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask28_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask28.Checked)
            {
                Result |= 1 << 27;
            }
            else
            {
                int temp = 1 << 27;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask29_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask29.Checked)
            {
                Result |= 1 << 28;
            }
            else
            {
                int temp = 1 << 28;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask30_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask30.Checked)
            {
                Result |= 1 << 29;
            }
            else
            {
                int temp = 1 << 29;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask31_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask31.Checked)
            {
                Result |= 1 << 30;
            }
            else
            {
                int temp = 1 << 30;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void Mask32_CheckedChanged(object sender, EventArgs e)
        {
            if (Mask32.Checked)
            {
                Result |= 1 << 31;
            }
            else
            {
                int temp = 1 << 31;
                Result &= ~temp;
            }
            MaskReslut.Text = Result.ToString();
        }

        private void ResetButtom_Click(object sender, EventArgs e)
        {
            Mask1.Checked = false;
            Mask2.Checked = false;
            Mask3.Checked = false;
            Mask4.Checked = false;
            Mask5.Checked = false;
            Mask6.Checked = false;
            Mask7.Checked = false;
            Mask8.Checked = false;
            Mask9.Checked = false;
            Mask10.Checked = false;
            Mask11.Checked = false;
            Mask12.Checked = false;
            Mask13.Checked = false;
            Mask14.Checked = false;
            Mask15.Checked = false;
            Mask16.Checked = false;
            Mask17.Checked = false;
            Mask18.Checked = false;
            Mask19.Checked = false;
            Mask20.Checked = false;
            Mask21.Checked = false;
            Mask22.Checked = false;
            Mask23.Checked = false;
            Mask24.Checked = false;
            Mask25.Checked = false;
            Mask26.Checked = false;
            Mask27.Checked = false;
            Mask28.Checked = false;
            Mask29.Checked = false;
            Mask30.Checked = false;
            Mask31.Checked = false;
            Mask32.Checked = false;
        }

    }
}
