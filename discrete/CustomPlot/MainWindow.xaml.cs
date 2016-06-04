using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
using Microsoft.Win32;
using OxyPlot;
using OxyPlot.Series;
using Quantpressor;
using Quantpressor.Distributions;
using Quantpressor.FileIO;

namespace CustomPlot
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		private readonly PlotModel plot;

		public MainWindow( )
		{
			InitializeComponent( );
			plot = new PlotModel( );
			InitPlot( );
		}

		public async void InitPlot( )
		{
			var reader = new CsvGridReader( 1024, ';' );

			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "Text files|*.csv",
				ValidateNames = true
			};

			var column = 0;

			var fileName = openFileDialog.ShowDialog( ) == true ? openFileDialog.FileName : null;

			if ( fileName == null )
			{ return; }

			var grid = await Task<IGrid>.Factory.StartNew( ( ) => reader.Read( fileName, false, false ) );

			double left = double.MaxValue, right = double.MinValue;

			for ( int i = 0; i < grid.RowCount; ++i )
			{
				var value = grid.GetValue( i, column );
				left = left < value ? left : value;
				right = right > value ? right : value;
			}

			var quantizer = new Quantizer( left, right );

			var empirical = new EmpiricalDistribution( grid, column );

			var q = await Task<IQuantization>.Factory.StartNew( ( ) => quantizer.Quantize( 15, 1e-3, empirical ) );

			var zero = new LineSeries
			{
				Color = OxyColor.FromRgb( 0, 0, 0 ),
				StrokeThickness = 1
			};
			zero.Points.Add( new DataPoint( left, 0 ) );
			zero.Points.Add( new DataPoint( right, 0 ) );
			plot.Series.Add( zero );

			var func = new FunctionSeries( x => empirical.Density( x ), left, right, 1e-2 );
			plot.Series.Add( func );

			foreach ( var border in q.Borders )
			{
				var line = new LineSeries
				{
					LineStyle = LineStyle.Dash,
					Color = OxyColor.FromRgb( 0, 0, 0 ),
					StrokeThickness = 1
				};
				line.Points.Add( new DataPoint( border, 3e-1 ) );
				line.Points.Add( new DataPoint( border, -3e-2 ) );
				plot.Series.Add( line );
			}

			foreach ( var code in q.Codes )
			{
				var line = new LineSeries
				{
					LineStyle = LineStyle.Dash,
					Color = OxyColor.FromRgb( 140, 140, 140 ),
					StrokeThickness = 0.5
				};
				line.Points.Add( new DataPoint( code, 3e-1 ) );
				line.Points.Add( new DataPoint( code, -3e-2 ) );
				plot.Series.Add( line );
			}

			var codes = from code in q.Codes
			            select new ScatterPoint( code, empirical.Density( code ) );

			var points = new ScatterSeries
			{
				MarkerType = MarkerType.Circle,
				MarkerStroke = OxyColor.FromRgb( 2, 133, 230 )/*( 255, 0, 0 )*/,
				MarkerFill = OxyColor.FromRgb( 2, 133, 230 )/*( 255, 115, 41 )*/
			};
			points.Points.AddRange( codes );

			plot.Series.Add( points );

			PlotView.Model = plot;
		}
	}
}
