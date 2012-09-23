#pragma once

#ifndef MATRIX_H
#define MATRIX_H

#include <utility>
#include <vector>

class Matrix
{
	public:
		Matrix(std::size_t row, std::size_t col);
		Matrix operator *(const Matrix & other);
		const std::vector<float> & operator[](std::size_t rowId) const;
		inline std::size_t row() const{ return data.size(); }
		inline std::size_t col() const{ return data.empty()?0:data[0].size(); } 
		
		void print() const;
	private:
		std::vector<std::vector<float>> data;
};

#endif
