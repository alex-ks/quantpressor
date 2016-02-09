using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using Caliburn.Micro;
using OxyPlot;
using OxyPlot.Series;

namespace Quantpressor.UI.ViewModels
{
	public class CompressionResultViewModel : PropertyChangedBase
	{
		public DataTable Result { get; set; }

		public BindableCollection<TabItem> Plots { get; }

		public CompressionResultViewModel( ICompressionResult compressionResult,
		                                   double[] leftBorders,
		                                   double[] rightBorders,
										   IList<IQuantization> quantizations,
										   IList<IDistribution> distributions )
		{
			Result = new DataTable( "Compression result" );
			Result.Columns.Add( "Parameter" );

			for ( int i = 0; i < leftBorders.Length; ++i )
			{
				Result.Columns.Add( $"Column #{i + 1}" );
			}

			var bordersList = new List<string> {"Values interval"};
			bordersList.AddRange( leftBorders.Select( ( t, i ) => $"[{t}; {rightBorders[i]}]" ) );
			Result.Rows.Add( bordersList );

			var entropyList = new List<string> {"Entropy"};
			entropyList.AddRange( quantizations.Select( q => $"{q.Entropy}" ) );
			Result.Rows.Add( entropyList );

			var bpsVariances = new List<string> { "Real variance" };
			bpsVariances.AddRange( compressionResult.ColumnsBitsPerSymbol.Select( d => $"{d}" ) );
			Result.Rows.Add( bpsVariances );

			var varianceList = new List<string> { "Expected variance" };
			varianceList.AddRange( quantizations.Select( q => $"{q.Variance}" ) );
			Result.Rows.Add( varianceList );

			var realVariances = new List<string> {"Real variance"};
			realVariances.AddRange( compressionResult.RealVariances.Select( d => $"{d}" ) );
			Result.Rows.Add( realVariances );

			var minErrors = new List<string> {"Minimal error"};
			minErrors.AddRange( compressionResult.MinErrors.Select( d => $"{d}" ) );
			Result.Rows.Add( minErrors );

			var maxErrors = new List<string> { "Maximal error" };
			maxErrors.AddRange( compressionResult.MaxErrors.Select( d => $"{d}" ) );
			Result.Rows.Add( maxErrors );

			var avgErrors = new List<string> { "Avgimal error" };
			avgErrors.AddRange( compressionResult.AvgErrors.Select( d => $"{d}" ) );
			Result.Rows.Add( avgErrors );

			Plots = new BindableCollection<TabItem>( );
			for ( int i = 0; i < leftBorders.Length; ++i )
			{
				PlotModel model = new PlotModel( )
				{
					Title = $"Column #{i}"
				};

				var func = new FunctionSeries( distributions[i].Density, leftBorders[i], rightBorders[i], 1e-1, "Empirical density" )
				{
					Smooth = true,
					LineStyle = LineStyle.Solid
				};

				model.Series.Add( func );
				model.Subtitle = $"Expectation = {distributions[i].TotalExpectation}, Variance = {distributions[i].TotalVariance}";

				Plots.Add( new TabItem { Content = new PlotViewModel { Model = model}, Header = model.Title });
			}
		}
	}
}
