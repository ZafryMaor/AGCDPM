#include "Statistics.h"

void init_log_facs(double* log_facs, double n) {
	log_facs[0] = 0;
	for (int i = 1; i < n + 1; ++i) {
		log_facs[i] = log_facs[i - 1] + log((double)i);
	}
}

double log_hypergeometric_prob(double* log_facs, int a, int b, int c, int d) {
	return log_facs[a + b] + log_facs[c + d] + log_facs[a + c] + log_facs[b + d]
		- log_facs[a] - log_facs[b] - log_facs[c] - log_facs[d] - log_facs[a + b + c + d];
}

double calculate_pval(double a, double b, double c, double d, double* log_facs) {
	double n = a + b + c + d;

	double logp_cutoff = log_hypergeometric_prob(log_facs, (int)a, (int)b, (int)c, (int)d);
	double p_fraction = 0;
	for (int x = 0; x <= n; ++x) {
		if (a + b - x >= 0 && a + c - x >= 0 && d - a + x >= 0) {
			double l = log_hypergeometric_prob(log_facs, x, (int)a + (int)b - x, (int)a + (int)c - x, (int)d - (int)a + x);
			if (l <= logp_cutoff) p_fraction += exp(l - logp_cutoff);
		}
	}
	double logp_value = logp_cutoff + log(p_fraction);

	return exp(logp_value);
}

long double calculate_qval(double i, double N, double pval) {
	return pval * N / i;
}

void calculate_statistical_significance(std::vector<std::pair<Feature, Statistics>> & patterns_statistical_significance,
	double origins_labeld_1_counter, double number_of_origins, double number_of_trails) {
	double* log_facs = new double[(int)number_of_origins + 1]; 
	init_log_facs(log_facs, number_of_origins);

	for (auto iter = patterns_statistical_significance.begin(); iter != patterns_statistical_significance.end(); ++iter) {
		
		double origins_labeld_0_counter = number_of_origins - origins_labeld_1_counter;
		
		double pat_sup_label_0 = (double)iter->first.sup_0;
		double pat_sup_label_1 = (double)iter->first.sup_1;
		double c = origins_labeld_0_counter - pat_sup_label_0;
		double d = origins_labeld_1_counter - pat_sup_label_1;
		
		double pval = calculate_pval(pat_sup_label_0, pat_sup_label_1, c, d, log_facs);
		iter->second.pval = pval;
	}

	delete[] log_facs;

	std::sort(patterns_statistical_significance.begin(), patterns_statistical_significance.end(), [](const std::pair<Feature, Statistics> a, const std::pair<Feature, Statistics> b) -> bool
		{
			long double l = a.second.pval;
			long double r = b.second.pval;
			return l > r;
		});

	double i = patterns_statistical_significance.size();
	for (auto iter = patterns_statistical_significance.begin(); iter != patterns_statistical_significance.end(); ++iter) {
		double pval = iter->second.pval;
		double qval = calculate_qval(i, (double)number_of_trails, pval);
		iter->second.qval = qval;
		i = i - 1;
	}

}

Statistics get_statistics(Itemset& itemset, std::vector<std::pair<Feature, Statistics>>& patterns_statistical_significance) {
	for (auto feature_statistics : patterns_statistical_significance) {
		if (are_patterns_equal(feature_statistics.first.itemset, itemset)) {
			return feature_statistics.second;
		}
	}
	return Statistics(0,0);
}