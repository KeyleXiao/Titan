/*******************************************************************
** 文件名:	IntervalTree.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭璐
** 日  期:	2012/10/11
** 版  本:	1.0
** 描  述:	区间树
** 应  用:	
** 说  明： 本算法是在红黑树的基础上扩展开来的，加入了low和high两个域来表示interval的上下限，并通过flag标记是否存在覆盖区
** 注  意:  为了提高效率，在构建R-B区间树完成之后，需要主动调用Fix函数来保证查找的正确性
********************************************************************/
#ifndef __IntervalTree_h__
#define __IntervalTree_h__

#include <iostream>
#include <cstdlib>

using namespace std;

//#define INTERVALTREE_USE_SIZE		// 是否支持大小Size功能

namespace rkt
{
	typedef unsigned long RBTREE_KEY;

	// 区间树C++类实现
	class CIntervalTree
	{
	public:
		enum RBcolor
		{
			INTERVALTREE_RED,
			INTERVALTREE_BLACK
		};  // 红黑树颜色

		//定义interval域的low和high
		struct Field
		{
			RBTREE_KEY low;
			RBTREE_KEY high;
		};

		/* 定义红黑树节点 */
		typedef struct RB_treeNode 
		{
			BYTE color;
			RBTREE_KEY key;
			RB_treeNode * left;			// 指向左孩子节点
			RB_treeNode * right;		// 指向右孩子节点
			RB_treeNode * parent;		// 指向父节点
#ifdef INTERVALTREE_USE_SIZE
			int size;					// 容量
#endif
			Field field;				// 域
			RBTREE_KEY max;				// max(high[int[x]],max[leftx]],max[right[x]])
		}*PRB_treeNode;

	private:
		PRB_treeNode m_root;		// 根节点指针

	public:
		CIntervalTree()
		{
			m_root = NULL;
		}

		~CIntervalTree()
		{
			clear(); 
		}

		// 插入节点
		/*
		功能：向树中插入节点
		参数：根结点root，待插入节点x的key和high
		*/
		PRB_treeNode RBinsert(RBTREE_KEY key, RBTREE_KEY low, RBTREE_KEY high)  //插入节点值为key
		{
			PRB_treeNode y, x;
			PRB_treeNode z;

			if (!(z = (RB_treeNode *)malloc(sizeof(RB_treeNode))))  //动态分配空间
			{
				return NULL;
			}

			z->key = key; 
			(z->field).low = low;
			(z->field).high = high;
			z->max = high;

#ifdef INTERVALTREE_USE_SIZE
			z->size = 1;
#endif

			y = NULL;
			x = m_root;
			while (x != NULL)
			{
				y = x;
				if (z->field.low < x->field.low)
					if(x->left != NULL)
					{
#ifdef INTERVALTREE_USE_SIZE
						x->size = x->size + 1; //父节点的数量加1
#endif
						x = x->left;
					}
					else
					{
#ifdef INTERVALTREE_USE_SIZE
						x->size = x->size + 1; //父节点的数量加1
#endif
						break;
					}
				else
					if(x->right != NULL)
					{
#ifdef INTERVALTREE_USE_SIZE
						x->size = x->size + 1; //父节点的数量加1
#endif
						x = x->right;
					}
					else
					{
#ifdef INTERVALTREE_USE_SIZE
						x->size = x->size + 1; //父节点的数量加1
#endif
						break;
					}
			}

			z->parent = y;
			// 将z插入到合适的位置
			if (y == NULL)
			{
				m_root = z;
			}
			else if (z->field.low < y->field.low)
			{
				y->left = z;
			}
			else
			{
				y->right = z;
			}

			// 设置z的左右子树为空并且颜色是red，注意新插入的节点颜色都是red
			z->left = NULL;
			z->right = NULL;
			z->color = INTERVALTREE_RED;

			// 调整节点使满足红黑树条件
			return RBinsertfixup(z);
		}

		// 调整节点的max域
		void Fix()
		{
			if (m_root != NULL)
			{
				Fixtree(m_root);
			}
		}

		// 区间树查找
		/*
		 操作： 区间树查找
		 参数： 树根，待查找的域
		 输出： 如有输出节点的key
		*/
		PRB_treeNode Interval_Search(Field i) //区间树查找
		{
			if(NULL == m_root)
			{
				return NULL;
			}
			// 树根
			PRB_treeNode x = m_root;
			// 标志是否存在覆盖，存在为1，否则为0
			int flag = 0;

			// 存在覆盖条件
			if ((i.low <= (x->field).high) && ((x->field).low <= i.high))
				flag = 1;
			while(x != NULL && flag == 0)
			{
				if((x->left != NULL) && (x->left->max >= i.low))
					x = x->left;
				else
					x = x->right;

				// 存在覆盖条件
				if(x != NULL)
				{
					if ((i.low <= (x->field).high) && ((x->field).low <= i.high))
						flag = 1;
				}
			}

			// 如是NULL则就是没有覆盖
			return x;
		}


		// 清空所有节点
		void clear()
		{
			if (m_root == NULL)
			{
				return;
			}

			RemoveNode(m_root);
			m_root = NULL;
		}

	private:
		void RemoveNode(PRB_treeNode pNode)
		{
			if (pNode == NULL)
			{
				return;
			}

			RemoveNode(pNode->left);
			RemoveNode(pNode->right);
			free(pNode);
		}

		// 调整节点的max域
		/*
		 功能：调整树的max域
		 参数：节点x
		 */
		/*
		RBTREE_KEY Fixtree(PRB_treeNode &x)
		{
			x->max = (x->field).high;
			RBTREE_KEY temp;

			if (x->left != NULL)
			{
				temp = Fixtree(x->left);
				if (temp > x->max)
				{
					x->max = temp;
				}
			}
			if (x->right != NULL)
			{
				temp = Fixtree(x->right);
				if (temp > x->max)
				{
					x->max = temp;
				}
			}

			return x->max;
		}
		*/
		void Fixtree(PRB_treeNode &x)  //调整树的max域
		{
			RBTREE_KEY max;
			if (x->left != NULL)
				Fixtree(x->left);
			if (x->right != NULL)
				Fixtree(x->right);

			if (x->left == NULL && x->right !=NULL)
			{
				x->max = (((x->field).high) >= (x->right->max)) ? ((x->field).high) : (x->right->max) ;
			}
			else if(x->left != NULL && x->right == NULL)
			{
				x->max = (((x->field).high) >= (x->left->max)) ? ((x->field).high) : (x->left->max) ;
			}
			else if(x->left != NULL && x->right !=NULL)
			{
				max = (((x->field).high) >= (x->left->max)) ? ((x->field).high) : (x->left->max) ;
				x->max = (max >= (x->right->max)) ? max : (x->right->max) ;
			}
		}

		// 左边旋节点x
		void LeftRotate(PRB_treeNode x)
		{
			PRB_treeNode y;
			y = x->right;
			if(!y)
				return;
			x->right = y->left;
			if (y->left!=NULL)
				y->left->parent = x;
			y->parent = x->parent;

			if (x == m_root)
			{
				m_root = y;
			}
			else if (x == x->parent->left)
			{
				x->parent->left = y;
			}
			else
				x->parent->right = y;

			y->left = x;
			x->parent = y;

#ifdef INTERVALTREE_USE_SIZE
			y->size = x->size;              //将x的size赋给y
			if(x->left==NULL&&x->right!=NULL)
				x->size = x->right->size + 1;  //改变x的size
			else if(x->right==NULL && x->left !=NULL)
				x->size = x->left->size + 1;
			else if(x->left!=NULL&&x->right!=NULL)
				x->size = x->left->size + x->right->size +1;
			else if(x->left==NULL && x->right ==NULL)
				x->size = 1;
#endif
		}

		// 右旋节点x
		// 以x节点为轴进行右旋
		void RightRotate(PRB_treeNode x)
		{
			RB_treeNode *y;

			y = x->left;
			if(y == NULL)
			{
				return;	
			}

			x->left = y->right;
			if(y->right!=NULL)
				y->right->parent = x;
			y->parent = x->parent;

			if (x == m_root)
				m_root = y;
			else if (x == x->parent->left)
				x->parent->left = y;
			else
				x->parent->right = y;

			y->right = x;
			x->parent = y;

#ifdef INTERVALTREE_USE_SIZE
			y->size = x->size;
			if(x->left==NULL&&x->right!=NULL)  //改变x的size
				x->size = x->right->size + 1;  
			else if(x->right==NULL && x->left !=NULL)
				x->size = x->left->size +1;
			else if(x->left!=NULL&&x->right!=NULL)
				x->size = x->left->size + x->right->size + 1;
			else if(x->left==NULL && x->right ==NULL)
				x->size = 1;
#endif
		}

		// 访问节点 ，以便输出节点信息
		//void Visittree(PRB_treeNode root);

		// 打印节点信息
		//void Print(PRB_treeNode x);

		// 调整节点
		/*
		功能：调整节点使满足红黑树条件
		参数：树根root，待调整节点z
		*/
		PRB_treeNode RBinsertfixup(PRB_treeNode z)
		{
			PRB_treeNode y;

			// 当z不是根同时父节点的颜色是red
			while ((m_root != z) && (z->parent->color == INTERVALTREE_RED))
			{
				// 父节点是祖父节点的左子树
				if (z->parent == z->parent->parent->left)
				{
					// y为z的伯父节点
					y = z->parent->parent->right;
					// 伯父节点存在且颜色是red
					if (y != NULL && y->color == INTERVALTREE_RED)
					{
						// 更改z的父节点颜色置位为Balck
						z->parent->color = INTERVALTREE_BLACK;					// case 1
						// 更改z的伯父节点颜色是Black
						y->color = INTERVALTREE_BLACK;
						// 更改z的祖父节点颜色是Red
						z->parent->parent->color = INTERVALTREE_RED;
						z = z->parent->parent;
					}
					else
					{
						if (z == z->parent->right)
						{
							z = z->parent;
							LeftRotate(z);							// case 2
						}

						z->parent->color = INTERVALTREE_BLACK;					// case 3
						z->parent->parent->color = INTERVALTREE_RED;
						RightRotate(z->parent->parent);
					}
				}
				// 父节点为祖父节点的右子树
				else
				{
					y = z->parent->parent->left;
					if (y != NULL && y->color == INTERVALTREE_RED)
					{
						z->parent->color = INTERVALTREE_BLACK;					// case 4
						y->color = INTERVALTREE_BLACK;
						z->parent->parent->color = INTERVALTREE_RED;
						z = z->parent->parent;
					}
					else
					{
						if (z == z->parent->left)
						{
							z = z->parent;
							RightRotate(z);					// case 5
						}
						z->parent->color = INTERVALTREE_BLACK;
						z->parent->parent->color = INTERVALTREE_RED;				// case 6
						LeftRotate(z->parent->parent);
					}
				}
			}

			// 根节点的颜色始终都是Black
			m_root->color = INTERVALTREE_BLACK;
			return m_root;
		}


		/* 测试用代码暂注释
		// 功能：访问节点
		// 参数：节点root
		void Visittree(PRB_treeNode root)       //中序遍历红黑树,以便打印节点信息
		{
			PRB_treeNode x = root;

			if (x != NULL)
			{
				if (x->left)
					Visittree(x->left);
				Print(x);
				if (x->right)
					Visittree(x->right);	   

			}
		}

		// 参数：节点x
		// 输出：节点x的信息
		void Print(PRB_treeNode x)   //打印节点的信息
		{
			cout<<"key: "<<x->field.low<<"\tlow: "<<(x->field).low<<"\thigh: "<<(x->field).high<<"\tmax: "<<x->max<<"\tcolor: "<<x->color<<"  parent: ";
			if(x->parent!=NULL)
				cout<<x->parent->field.low;
			else
				cout<<"NULL";
			cout<<"\tleft: ";
			if(x->left!=NULL)
				cout<<x->left->field.low;
			else
				cout<<"NULL";
			cout<<"\tright: ";
			if(x->right!=NULL)
				cout<<x->right->field.low;
			else
				cout<<"NULL";
		#ifdef INTERVALTREE_USE_SIZE
			cout<<"\tsize:"<<x->size;
		#endif
			if(x->left==NULL&&x->right==NULL)
				cout<<"\tis Leaves";
			if(x->parent==NULL)
				cout<<"\tis root";
			cout<<endl;
			cout<<"-------------------"<<endl;
		}
		*/
	};

} // namespace


/* 测试代码
int main()
{
	// 树根节点，nill节点这里就时NULL
	PRB_treeNode  root = NULL;
	PRB_treeNode  returnvalue;
	int low, high;
	Field interval;
	cout << "please input node information low and high :"
		 << endl
		 << "input low = 0 to stop insert node"
		 << endl;

	do
	{
	    cout << "low:";
		cin >> low;

		cout << "high:";
		cin >> high;

		// 插入节点
		if (low <= high)
		{
			root = RBinsert(root, low, high);
		}
	    cout << endl;
	}
	while (low != 0);

	if(root)
	{
		// 调整节点的max域
		Fixtree(root);
	}

	int i;
	// 输入0 打印打印节点信息，1 区间树查询
	cout << "please input node information(0/1):";
	cin >> i;
	if(!i)
	{
		// 输出节点信息
		Visittree(root);
		cout << endl;
	}
	else
	{
		cout << "please input a interval to search(low, high): \n";
		cout << "low:";
		cin >> interval.low;
		cout << "high:";
		cin >> interval.high;

		// 输出结果
		if (returnvalue = Interval_Search(root, interval))
		{
			cout << "the result node key is " << returnvalue->field.low << endl;
		}
		else
		{
			cout << "there is no interval !" << endl;
		}
	}

	return 0;
}
*/



#endif // __IntervalTree_h__