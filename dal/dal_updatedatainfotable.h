/*
 * dal_updatedatainfotable.h
 *
 *  Created on: 2012-4-27
 *      Author: jimm
 */

#ifndef DAL_UPDATEDATAINFOTABLE_H_
#define DAL_UPDATEDATAINFOTABLE_H_


//数据成员更新信息表的大小
#define UpdateDataInfoTableSize		128

class UpdateDataInfoTable
{
public:
	UpdateDataInfoTable()
	{
		nOffset = 0;
		nDataSize = 0;
	}

	uint32_t	nOffset;
	uint32_t	nDataSize;
};

#define GetMemberOffset(member)		((uint32_t)(/*GetStartPos() + */(((uint8_t *)&(member)) - ((uint8_t *)&m_nStartPos))))


#endif /* DAL_UPDATEDATAINFOTABLE_H_ */
