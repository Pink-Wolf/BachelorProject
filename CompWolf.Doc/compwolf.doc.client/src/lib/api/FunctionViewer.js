import { Declaration } from "../CodeComponents";
import FormattedText from "../FormattedText";
import BaseEntityViewer from "./BaseEntityViewer";
import { Fragment } from "react";

export default function FunctionViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<ol id="Declarations" type="1">
					{data.overloads.map((x, i) => {
						return (
							<li key={i + 1}>
								<Declaration>{x.declaration}</Declaration>
								<FormattedText>{x.description}</FormattedText>
							</li>
						)
					})}
					{data.name !== data.owner}
				</ol>
				<p id="Description">
					<FormattedText>{data.detailedDescription ?? ""}</FormattedText>
				</p>
			</Fragment>
		}>

			<section hidden={is_empty(data.exceptions)} id="Exceptions">
				<h2>Exceptions</h2>
				{data.exceptions?.map((x, i) => {
					return <blockquote key={i}><FormattedText>{x}</FormattedText></blockquote>
				})}
			</section>
		</BaseEntityViewer>
	)
}