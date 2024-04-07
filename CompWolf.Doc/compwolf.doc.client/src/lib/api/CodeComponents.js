import Link from "next/link";
import SyntaxHighlighter from "react-syntax-highlighter/dist/esm/default-highlight";
import { xcode } from "react-syntax-highlighter/dist/esm/styles/hljs";

export function Reference(props) {
	return (
		<Link href={"/api/" + props.path}>
			<code>
				{props.children}
			</code>
		</Link>
	)
}

export function CodeViewer(props) {
	return (
		<SyntaxHighlighter language="cpp" style={xcode}>
			{props.children}
		</SyntaxHighlighter>
	)
}
export function Declaration(props) {
	return (
		<big className="declaration">
			<CodeViewer>
				{props.children}
			</CodeViewer>
		</big>
	)
}
