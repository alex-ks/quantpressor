using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Caliburn.Micro;
using Microsoft.Win32;
using Quantpressor.Compressors;
using Quantpressor.Distributions;
using Quantpressor.FileIO;

namespace Quantpressor.UI.ViewModels
{
	class MainViewModel : PropertyChangedBase
	{
		private IWindowManager _manager;

		private double _error = 1e-2;
		public string Error
		{
			get { return _error.ToString( ); }
			set { _error = double.Parse( value ); }
		}

		private int _quantCount = 100;
		public string QuantCount
		{
			get { return _quantCount.ToString( ); }
			set { _quantCount = int.Parse( value ); }
		}

		public MainViewModel( IWindowManager manager )
		{
			_manager = manager;
		}

		private IGrid OpenGrid( )
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "Text files|*.csv",
				ValidateNames = true
			};

			if ( openFileDialog.ShowDialog( ) != true )
				return null;

			var reader = new CsvGridReader( 1024 * 1024, ';' );
			return reader.Read( openFileDialog.FileName, false, false );
			//var openTask = new Task<IGrid>( ( ) => reader.Read( openFileDialog.FileName, false, false ) );
			//openTask.Start( );
			//return await openTask;
		}

		private void CompressAndShow( IGrid grid, string outName, ProgressViewModel progressBar )
		{
			var distrs = new List<IDistribution>( );
			double[] leftBorders = new double[grid.ColumnCount];
			double[] rightBorders = new double[grid.ColumnCount];

			var quantizations = new List<IQuantization>( );

			for ( int column = 0; column < grid.ColumnCount; ++column )
			{
				progressBar.Status = $"Quantizing column #{column + 1}...";
				progressBar.Progress = ( double )column / ( grid.ColumnCount + 1 );

				var distr = new EmpiricalDistribution( grid, column );
				distrs.Add( distr );

				leftBorders[column] = double.MaxValue;
				rightBorders[column] = double.MinValue;

				for ( int row = 0; row < grid.RowCount; ++row )
				{
					double value = grid.GetValue( row, column );
					leftBorders[column] = leftBorders[column] < value ? leftBorders[column] : value;
					rightBorders[column] = rightBorders[column] > value ? rightBorders[column] : value;
				}

				var quantizer = new Quantizer( leftBorders[column], rightBorders[column] );
				quantizations.Add( quantizer.Quantize( _quantCount, _error, distr ) );
			}

			progressBar.Status = "Writing archive...";
			progressBar.Progress = ( double )grid.ColumnCount / ( grid.ColumnCount + 1 );

			ICompressionResult result;

			using ( var stream = new FileOutputStream( outName ) )
			{
				var compressor = new HuffmanCompressor( );
				result = compressor.Compress( grid, quantizations, stream );
			}

			MessageBox.Show( "Compressed" );

			progressBar.Progress = 1.0;
			progressBar.TryClose( );

			var resultViewModel = new CompressionResultViewModel( result, leftBorders, rightBorders, quantizations, distrs );
			_manager.ShowWindow( resultViewModel );
		}

		public void Compress( )
		{
			var grid = OpenGrid( );

			if ( grid == null )
				return;

			var dialog = new SaveFileDialog
			{
				DefaultExt = "out",
				Filter = "Quantized and compressed grid|*.out",
				AddExtension = true
			};

			if ( dialog.ShowDialog( ) != true )
				return;

			string outName = dialog.FileName;

			var progressBar = new ProgressViewModel( );

			dynamic settings = new ExpandoObject( );
			settings.Height = 100;
			settings.Width = 500;
			settings.SizeToContent = SizeToContent.Manual;

			_manager.ShowWindow( progressBar, null, settings );

			CompressAndShow( grid, outName, progressBar );

			//var compressTask = new Task( ( ) =>
			//{
			//	CompressAndShow( grid, outName, progressBar );
			//} );

			//compressTask.Start( );

			//await compressTask;
		}
	}
}
