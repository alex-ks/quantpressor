using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Quantpressor.UI
{
	public struct CompressionStats
	{
		public double[] LeftBorders { get; set; }
		public double[] RightBorders { get; set; }

		public List<IQuantization> Quantizations { get; set; }
		public List<IDistribution> Distributions { get; set; }

		public ICompressionResult CompressionResult { get; set; }
	}
}
