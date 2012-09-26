#pragma once

#ifndef MATRIX_H
#define MATRIX_H

#include <utility>
#include <vector>
#include <initializer_list>

class Matrix
{
	public:
		Matrix(){}
		Matrix(std::size_t row, std::size_t col);
		Matrix(std::initializer_list<std::vector<float>> values);
		Matrix operator *(const Matrix & other);
		std::vector<float> & operator[](std::size_t rowId);
		const std::vector<float> & operator[](std::size_t rowId) const;
		inline std::size_t row() const{ return data.size(); }
		inline std::size_t col() const{ return data.empty()?0:data[0].size(); } 
		
		void print() const;
	private:
		std::vector<std::vector<float>> data;
};

#endif
