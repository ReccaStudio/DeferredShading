#pragma once

template<class T>
class Singleton
{
public:
	Singleton()
		: m_pInstance(0)
	{

	}
// 	static void Create() {
// 		if (!m_pInstance)
// 			m_pInstance = new T;
// 	}
// 	static void Destroy() {
// 		if (m_pInstance)
// 			delete m_pInstance;
// 	}

	T* Instance() const {
		if (!m_pInstance)
			m_pInstance = new T;

		return m_pInstance;
	}

protected:
	T* m_pInstance;

};

// template<class T>
// T* Singleton<T>::m_pInstance = NULL;