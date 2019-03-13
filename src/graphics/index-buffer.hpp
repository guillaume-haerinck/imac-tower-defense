#pragma once

class IndexBuffer {
private:
	unsigned int m_rendererID;
	unsigned int m_count;

public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void init(const unsigned int* data, unsigned int count);
	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return m_count; }
	unsigned int getID() const { return m_rendererID; }
};
