/*
 */
import java.io.*;
import java.util.*;
import java.util.concurrent.ThreadLocalRandom;
import java.lang.Math;

public class BracketsToTree {
	private ThreadLocalRandom thr = ThreadLocalRandom.current();
	BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	public static void main( String [] args ) throws Exception {
		new BracketsToTree().go();
	}
	void go() throws Exception {
		Map<Integer,Set<Integer>> adj;
		int i,j,k,n,x,y;
		for ( String s; (s = br.readLine()) != null; ) {
			Stack<Integer> st = new Stack<>();
			adj = new HashMap<>();
			for ( n = 0, adj.put(n,new HashSet<>()), st.push(n), i = 0; i < s.length(); ++i ) {
				char ch = s.charAt(i);
				if ( ch == '(' ) {
					x = st.peek();
					st.push(++n);
					adj.put(n,new HashSet<>());
					adj.get(x).add(n);
					continue ;
				}
				assert ch == ')';
				st.pop();
			}
			++n;
			//int C = thr.nextInt(n,2*n);
			int C = thr.nextInt((int)(Math.log(n)/Math.log(2)),(int)(Math.log(2*n)/Math.log(2)));
			System.out.printf("%d %d\n",n,C);
			for ( i = 0; i < n; ++i )
				System.out.printf("%d ",thr.nextInt(1,C));
			System.out.println(""); k = 0;
			for ( Map.Entry<Integer,Set<Integer>> entry: adj.entrySet() ) {
				for ( Integer v: entry.getValue() )
					System.out.printf("%d %d\n",entry.getKey(),v);
				k += entry.getValue().size();
			}
			assert k == n-1;
		}
	}
}

