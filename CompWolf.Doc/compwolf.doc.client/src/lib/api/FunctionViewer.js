import { Declaration } from "./CodeComponents";
import BaseEntityViewer from "./BaseEntityViewer";

export default function FunctionViewer(props) {
	const data = props.data;

	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<BaseEntityViewer data={data} top={
			<ol id="Declarations" type="1">
				{data.overloads.map((x, i) => {
					return (
						<li key={i + 1}>
							<Declaration>{x.declaration}</Declaration>
							{x.description}
						</li>
					)
				})}
			</ol>
		}>
			<section hidden={is_empty(data.exceptions)} id="Exceptions">
				<h2>Exceptions</h2>
				{data.exceptions?.map((x, i) => {
					return <blockquote key={i}>{x}</blockquote>
				})}
			</section>
		</BaseEntityViewer>
	)
}