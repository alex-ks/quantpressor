using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Quantpressor.UI.Views
{
	/// <summary>
	/// Interaction logic for CompressionParamsView.xaml
	/// </summary>
	public partial class CompressionParamsView : UserControl
	{
		public CompressionParamsView( )
		{
			InitializeComponent( );
		}

		private void ErrorBox_OnPreviewTextInput( object sender, TextCompositionEventArgs e )
		{
			e.Handled = !IsNumeric( e.Text );
		}

		private static bool IsNumeric( string text )
		{
			Regex regex = new Regex( "[^0-9.]+" ); //regex that matches disallowed text
			return !regex.IsMatch( text );
		}
	}
}
