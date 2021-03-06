﻿/// \file		VSingleton.h
/// \author		Daniel Huc, Henry Lefèvre
/// \date		September 2016
/// \breif		Inspired by : http://tfc.duke.free.fr/coding/singleton.html
#pragma once

namespace Utility {

	/* Singleton Pattern */
	template <typename T>
	class VSingleton {
	/* Members */
	protected:
		static T* m_instance;

	/* Methods */
	protected:
		VSingleton();
		virtual ~VSingleton() = 0;	// Pure virtual : this is a virtual class

	public:
		static T& getInstance() {
			if (T::m_instance == nullptr)
				T::m_instance = new T();
			return *m_instance;
		}
		static void destroyInstance() {
			if (T::m_instance != nullptr) {
				delete T::m_instance;
				T::m_instance = nullptr;
			}
		}
	};

	// Global variable initialisation:
	template <typename T>
	T* VSingleton<T>::m_instance = nullptr;
}