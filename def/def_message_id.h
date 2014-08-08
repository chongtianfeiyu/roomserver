/*
 * def_message_id.h
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#ifndef DEF_MESSAGE_ID_H_
#define DEF_MESSAGE_ID_H_

//��ҵ�����
#define MSGID_CLRS_HEARTBEAT					0x00018005
//��½��������
#define MSGID_CLRS_LOGIN_REQ					0x00018007

//��½������Ӧ
#define MSGID_RSCL_LOGIN_RESP					0x00020707

//��½����֪ͨ
#define MSGID_RSCL_LOGIN_NOTI					0x00030707

//�ǳ���������
#define MSGID_CLRS_LOGOUT_REQ					0x00018008

//�ǳ�����֪ͨ
#define MSGID_RSCL_LOGOUT_NOTI					0x00030708

//��ȡ�û��б�����
#define MSGID_CLRS_GETUSERLIST_REQ				0x00018009

//��ȡ�û��б���Ӧ
#define MSGID_RSCL_GETUSERLIST_RESP				0x00020709

//��ȡ�����б�����
#define MSGID_CLRS_GETWAITTINGLIST_REQ			0x0001800a

//��ȡ�����б���Ӧ
#define MSGID_RSCL_GETWAITTINGLIST_RESP			0x0002070a

//�����ı���Ϣ����
#define MSGID_CLRS_SENDTEXT_REQ					0x0001800b

//�����ı���Ϣ��Ӧ
#define MSGID_RSCL_SENDTEXT_RESP				0x0002070b

//�����ı���Ϣ֪ͨ
#define MSGID_RSCL_SENDTEXT_NOTI				0x0003070b

//��������
#define MSGID_CLRS_REQUESTSHOW_REQ				0x0001800e

//������Ӧ
#define MSGID_RSCL_REQUESTSHOW_RESP				0x0002070e

//����֪ͨ
#define MSGID_RSCL_REQUESTSHOW_NOTI				0x0003070e

//��������
#define MSGID_CLRS_EXITSHOW_REQ					0x00018010

//����֪ͨ
#define MSGID_RSCL_EXITSHOW_NOTI				0x00030710

//����رջ��ĳ�˵�����Ƶ
#define MSGID_CLRS_OPERATE_VIDEO_AUDIO_REQ		0x00018011

//��Ӧ�رջ��ĳ�˵�����Ƶ
#define MSGID_RSCL_OPERATE_VIDEO_AUDIO_RESP		0x00020711

//�رջ��ĳ�˵�����Ƶ��֪ͨ
#define MSGID_RSCL_OPERATE_VIDEO_AUDIO_NOTI		0x00030711

//��������
#define MSGID_CLRS_KICK_USER_REQ				0x00018012

//��Ӧ����
#define MSGID_RSCL_KICK_USER_RESP				0x00020712

//���˵�֪ͨ
#define MSGID_RSCL_KICK_USER_NOTI				0x00030712

//�Ӻ�����
#define MSGID_CLRS_ADD_BLACK_REQ				0x00018013

//��Ӧ�Ӻ�
#define MSGID_RSCL_ADD_BLACK_RESP				0x00020713

//�Ӻڵ�֪ͨ
#define MSGID_RSCL_ADD_BLACK_NOTI				0x00030713

//��IP����
#define MSGID_CLRS_LOCK_IP_REQ					0x00018014

//��Ӧ��IP
#define MSGID_RSCL_LOCK_IP_RESP					0x00020714

//��IP��֪ͨ
#define MSGID_RSCL_LOCK_IP_NOTI					0x00030714

//��������
#define MSGID_CLRS_SHUT_UP_REQ					0x00018015

//��Ӧ����
#define MSGID_RSCL_SHUT_UP_RESP					0x00020715

//���Ե�֪ͨ
#define MSGID_RSCL_SHUT_UP_NOTI					0x00030715

//�����������
#define MSGID_CLRS_UNLOCK_SHUT_UP_REQ			0x00018016

//��Ӧ�������
#define MSGID_RSCL_UNLOCK_SHUT_UP_RESP			0x00020716

//������Ե�֪ͨ
#define MSGID_RSCL_UNLOCK_SHUT_UP_NOTI			0x00030716

//������������
#define MSGID_CLRS_PUSH_ON_SHOW_REQ				0x00018017

//����������Ӧ
#define MSGID_RSCL_PUSH_ON_SHOW_RESP			0x00020717

//��������֪ͨ
#define MSGID_RSCL_PUSH_ON_SHOW_NOTI			0x00030717

//������������
#define MSGID_CLRS_PULL_OFF_SHOW_REQ			0x00018018

//����������Ӧ
#define MSGID_RSCL_PULL_OFF_SHOW_RESP			0x00020718

//��������֪ͨ
#define MSGID_RSCL_PULL_OFF_SHOW_NOTI			0x00030718

//���ӹ���Ȩ������
#define MSGID_CLRS_ADD_TITLE_REQ				0x00018019

//���ӹ���Ȩ����Ӧ
#define MSGID_RSCL_ADD_TITLE_RESP				0x00020719

//���ӹ���Ȩ��֪ͨ
#define MSGID_RSCL_ADD_TITLE_NOTI				0x00030719

//ɾ������Ȩ������
#define MSGID_CLRS_DEL_TITLE_REQ				0x0001801a

//ɾ������Ȩ����Ӧ
#define MSGID_RSCL_DEL_TITLE_RESP				0x0002071a

//ɾ������Ȩ��֪ͨ
#define MSGID_RSCL_DEL_TITLE_NOTI				0x0003071a

//��ĳ�˵�˽������
#define MSGID_CLRS_OPEN_PEIVATE_REQ				0x0001801b

//��ĳ�˵�˽����Ӧ
#define MSGID_RSCL_OPEN_PEIVATE_RESP			0x0002071b

//�ر�ĳ�˵�˽������
#define MSGID_CLRS_CLOSE_PEIVATE_REQ			0x0001801c

//�ر�ĳ�˵�˽����Ӧ
#define MSGID_RSCL_CLOSE_PEIVATE_RESP			0x0002071c

//������ĳ��һ��һ��Ƶ
#define MSGID_CLRS_SENDP2P_REQ					0x0001801d

//һ��һ��Ƶ��Ӧ����������ܵĽ����
#define MSGID_RSCL_SENDP2P_RESP					0x0002071d

//֪ͨ�Է���Ƶ���󣨱���������ʾ��������
#define MSGID_RSCL_SENDP2P_NOTI					0x0003071d

//һ��һ��Ƶ���ӵĴ�����
#define MSGID_CLRS_RECVP2P_REQ					0x0001801e

//һ��һ��Ƶ���ӵĴ𸴻�Ӧ
#define MSGID_RSCL_RECVP2P_RESP					0x0002071e

//֪ͨ��������������ս��
#define MSGID_RSCL_RECVP2P_NOTI					0x0003071e

//���÷�����Ϣ����
#define MSGID_CLRS_SET_ROOM_REQ					0x00018022

//���÷�����Ϣ��Ӧ
#define MSGID_RSCL_SET_ROOM_RESP				0x00020722

//֪ͨ���÷�����Ϣ
#define MSGID_RSCL_SET_ROOM_NOTI				0x00030722

//��ȡ������Ϣ����
#define MSGID_CLRS_GET_ROOM_REQ					0x00018023

//��ȡ������Ϣ��Ӧ
#define MSGID_RSCL_GET_ROOM_RESP				0x00020723

//֪ͨ���������˽��
#define MSGID_RSCL_SRV_PRIVATE_MIC_NOTI			0x00030728

//֪ͨ���䱻��
#define MSGID_RSCL_SRV_ROOM_LOCKED_NOTI			0x0003072b

//����mic����
#define MSGID_CLRS_SET_MIC_REQ					0x0001802c

//���÷�����Ϣ��Ӧ
#define MSGID_RSCL_SET_MIC_RESP					0x0002072c

//֪ͨ���÷�����Ϣ
#define MSGID_RSCL_SET_MIC_NOTI					0x0003072c

//֪ͨ��������media
#define MSGID_RSCL_RESET_MEDIA_NOTI				0x00030730

//֪ͨ��ҵ���Ϣ�ı�
#define MSGID_RSCL_USERINFO_CHANGE_NOTI			0x00030724

//Ӧ����������
#define MSGID_RSCL_ANSWER_PULL_SHOW_REQ			0x00018032

//Ӧ�����������Ӧ
#define MSGID_RSCL_ANSWER_PULL_SHOW_RESP		0x00020732

//���¸����б�����
#define MSGID_CLRS_UPDATESONGLIST_REQ			0x00018039

//���¸����б���Ӧ
#define MSGID_RSCL_UPDATESONGLIST_RESP			0x00020739

//��ȡ�����б�����
#define MSGID_CLRS_GETSONGLIST_REQ				0x0001803a

//��ȡ�����б���Ӧ
#define MSGID_RSCL_GETSONGLIST_RESP				0x0002073a

//�������
#define MSGID_CLRS_REQUESTSONG_REQ				0x0001803b

//���������Ӧ
#define MSGID_RSCL_REQUESTSONG_RESP				0x0002073b

//�������֪ͨ
#define MSGID_RSCL_REQUESTSONG_NOTI				0x0003073b

//����������
#define MSGID_CLRS_PROCESSORDER_REQ				0x0001803c

//��������Ӧ
#define MSGID_RSCL_PROCESSORDER_RESP			0x0002073c

//������֪ͨ
#define MSGID_RSCL_PROCESSORDER_NOTI			0x0003073c

//��ȡ��������
#define MSGID_CLRS_GETORDERLIST_REQ				0x0001803d

//��ȡ������Ӧ
#define MSGID_RSCL_GETORDERLIST_RESP			0x0002073d

//��ǰ�ĸ�����Ϣ֪ͨ
#define MSGID_RSCL_CURSONGINFO_NOTI				0x0003073e

//Ʊ������֪ͨ
#define MSGID_RSCL_TICKETCOUNTUPDATE_NOTI		0x0003073f

//��Ʊ����
#define MSGID_CLRS_FIGHTFORTICKET_REQ			0x00018040

//��Ʊ��Ӧ
#define MSGID_RSCL_FIGHTFORTICKET_RESP			0x00020740

//��ʼ����֪ͨ
#define MSGID_RSCL_ENJOYING_NOTI				0x00030741

//������������
#define MSGID_CLRS_SONGVOTE_REQ					0x0001803e

//����������Ӧ
#define MSGID_RSCL_SONGVOTE_RESP				0x0002073e

//����ͶƱ���֪ͨ
#define MSGID_RSCL_SONGVOTE_NOTI				0x0003073e

//����ͶƱ�������֪ͨ
#define MSGID_RSCL_SONGVOTEUPDATE_NOTI			0x00030743

//��ʼ�ȴ����֪ͨ
#define MSGID_RSCL_WAITINGFORPROGRAM_NOTI		0x00030744

//��ȡ�����б�����
#define  MSGID_CLRM_GETARTISTOFFLINE_REQ  0x00018041

//��ȡ�����б��Ӧ
#define  MSGID_CLRM_GETARTISTOFFLINE_RESP  0x00020741

#endif /* DEF_MESSAGE_ID_H_ */
