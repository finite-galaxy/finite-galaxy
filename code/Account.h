// Account.h

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include "Mortgage.h"

#include <cstdint>
#include <string>
#include <vector>

class DataNode;
class DataWriter;



// Class representing all your assets and liabilities and tracking their change
// over time.
class Account {
public:
  // Default constructor.
  Account();

  // Load or save account data.
  void Load(const DataNode &node);
  void Save(DataWriter &out) const;

  // Get or change the player's credits.
  int64_t Credits() const;
  void AddCredits(int64_t value);
  void PayExtra(int mortgage, int64_t amount);

  // Step forward one day, and return a string summarizing payments made.
  std::string Step(int64_t assets, int64_t salaries);

  // Overdue crew salaries:
  int64_t SalariesOwed() const;
  void PaySalaries(int64_t amount);

  // Liabilities:
  const std::vector<Mortgage> &Mortgages() const;
  void AddMortgage(int64_t principal);
  void AddFine(int64_t amount);
  int64_t Prequalify() const;
  // Assets:
  int64_t NetWorth() const;

  // Find out the player's credit rating.
  int CreditScore() const;
  // Get the total amount owed for "Mortgage", "Fine", or both.
  int64_t TotalDebt(const std::string &type = "") const;


private:
  int64_t YearlyRevenue() const;


private:
  int64_t credits;
  // If back salaries cannot be paid, they pile up rather than being ignored.
  int64_t salariesOwed;

  std::vector<Mortgage> mortgages;

  // History of the player's net worth. This is used to calculate your average
  // daily income, which is used to calculate how big a mortgage you can afford.
  std::vector<int64_t> history;
  // Your credit score determines the interest rate on your mortgages.
  int creditScore;
};



#endif
